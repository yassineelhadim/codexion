/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_ops.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 00:00:00 by yel-hadi       #+#    #+#             */
/*   Updated: 2026/09/11 00:00:00 by yel-hadi      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	request_is_queued(t_dongle *dongle, int coder_id)
{
	int	i;

	i = 0;
	while (i < dongle->waiting.size)
	{
		if (dongle->waiting.items[i].coder_id == coder_id)
			return (1);
		i++;
	}
	return (0);
}

static void	acquire_single_dongle(t_sim *sim, t_coder *coder)
{
	t_dongle	*dongle;
	t_request	request;

	dongle = &sim->dongles[coder->left];
	pthread_mutex_lock(&sim->lock);
	coder->state = STATE_WAITING_DONGLES;
	request.coder_id = coder->id;
	request.deadline = coder->deadline;
	request.arrival_seq = sim->next_arrival++;
	heap_push(&dongle->waiting, sim->config.scheduler, &request);
	grant_ready_pairs(sim);
	while (!sim->stop && !coder_has_both_dongles(sim, coder))
		pthread_cond_wait(&sim->event, &sim->lock);
	pthread_mutex_unlock(&sim->lock);
}

static void	take_dongle(t_sim *sim, t_coder *coder, int index)
{
	t_dongle	*dongle;
	t_request	request;
	int			other_queued;

	dongle = &sim->dongles[index];
	pthread_mutex_lock(&sim->lock);
	coder->state = STATE_WAITING_DONGLES;
	request.coder_id = coder->id;
	request.deadline = coder->deadline;
	request.arrival_seq = sim->next_arrival++;
	heap_push(&dongle->waiting, sim->config.scheduler, &request);
	grant_ready_pairs(sim);
	other_queued = request_is_queued(
			&sim->dongles[coder->left], coder->id);
	if (coder->left == index)
		other_queued = request_is_queued(
				&sim->dongles[coder->right], coder->id);
	if (other_queued)
	{
		while (!sim->stop && !coder_has_both_dongles(sim, coder))
			pthread_cond_wait(&sim->event, &sim->lock);
	}
	pthread_mutex_unlock(&sim->lock);
}

void	acquire_dongles(t_sim *sim, t_coder *coder)
{
	int	first;
	int	second;

	if (coder->left == coder->right)
	{
		acquire_single_dongle(sim, coder);
		return ;
	}
	else
	{
		if (coder->right > coder->left)
		{
			first = coder->left;
			second = coder->right;
		}
		else
		{
			first = coder->right;
			second = coder->left;
		}
	}
	take_dongle(sim, coder, first);
	if (!sim->stop)
		take_dongle(sim, coder, second);
}

void	withdraw_requests(t_sim *sim, t_coder *coder)
{
	t_dongle	*dongle;

	dongle = &sim->dongles[coder->left];
	heap_remove_coder(&dongle->waiting, sim->config.scheduler, coder->id);
	dongle = &sim->dongles[coder->right];
	heap_remove_coder(&dongle->waiting, sim->config.scheduler, coder->id);
}
