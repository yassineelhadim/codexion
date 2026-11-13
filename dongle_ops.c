/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*                                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <yel-hadi.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 00:00:00 by yel-hadi          #+#    #+#             */
/*   Updated: 2026/11/12 00:00:00 by yel-hadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	queue_request(t_sim *sim, t_dongle *dongle, t_coder *coder,
		long long arrival)
{
	t_request	request;

	request.coder_id = coder->id;
	request.deadline = coder->deadline;
	request.arrival_seq = arrival;
	heap_push(&dongle->waiting, sim->config.scheduler, &request);
}

void	lock_dongle_pair(t_dongle *left, t_dongle *right)
{
	if (left->id < right->id)
	{
		pthread_mutex_lock(&left->mutex);
		if (left != right)
			pthread_mutex_lock(&right->mutex);
	}
	else
	{
		pthread_mutex_lock(&right->mutex);
		if (left != right)
			pthread_mutex_lock(&left->mutex);
	}
}

void	unlock_dongle_pair(t_dongle *left, t_dongle *right)
{
	if (left->id < right->id)
	{
		if (left != right)
			pthread_mutex_unlock(&right->mutex);
		pthread_mutex_unlock(&left->mutex);
	}
	else
	{
		if (left != right)
			pthread_mutex_unlock(&left->mutex);
		pthread_mutex_unlock(&right->mutex);
	}
}

void	acquire_dongles(t_sim *sim, t_coder *coder)
{
	long long	arrival;
	t_dongle	*left;
	t_dongle	*right;

	pthread_mutex_lock(&sim->lock);
	coder->state = STATE_WAITING_DONGLES;
	left = &sim->dongles[coder->left];
	right = &sim->dongles[coder->right];
	lock_dongle_pair(left, right);
	arrival = sim->next_arrival++;
	queue_request(sim, left, coder, arrival);
	if (coder->left != coder->right)
		queue_request(sim, right, coder, arrival);
	unlock_dongle_pair(left, right);
	grant_ready_pairs(sim);
	while (!sim->stop && !coder_has_both_dongles(sim, coder))
		pthread_cond_wait(&sim->event, &sim->lock);
	pthread_mutex_unlock(&sim->lock);
}

void	withdraw_requests(t_sim *sim, t_coder *coder)
{
	t_dongle	*left;
	t_dongle	*right;

	left = &sim->dongles[coder->left];
	right = &sim->dongles[coder->right];
	lock_dongle_pair(left, right);
	heap_remove_coder(&left->waiting, sim->config.scheduler, coder->id);
	if (left != right)
		heap_remove_coder(&right->waiting, sim->config.scheduler, coder->id);
	unlock_dongle_pair(left, right);
}
