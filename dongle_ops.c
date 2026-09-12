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

/*
** Dongle lifecycle, everything under sim->lock:
**
**   acquire: enqueue both requests in ONE critical section (this atomic
**            double-enqueue is what makes the ring deadlock-free: a coder
**            can never be granted one dongle and wait forever for the
**            other one, because its two requests carry the same arrival
**            order on both queues), then wait until the grants put both
**            dongles in our hands.
**   release: put the dongles back on the table, arm their cooldown, and
**            let the next eligible waiter take over.
**
** Granting only ever hands a dongle to the waiter at the TOP of that
** dongle's heap, which is exactly the arbitration policy:
**   fifo -> arrival order,  edf -> earliest burnout deadline.
** A request whose dongle is free but still cooling down stays queued and
** is retried on the monitor's 1 ms broadcast tick; the entry is never
** dropped, so no waiter can be starved by later requests.
*/

static int	is_free_and_cooled(t_dongle *dongle)
{
	return (dongle->holder == -1 && time_in_ms() >= dongle->cooldown_until);
}

/*
** Single-coder case: there is only one dongle, so the coder can take one
** hand but can never complete the pair. It stays waiting until the monitor
** declares burnout.
*/
static void	acquire_single_dongle(t_sim *sim, t_coder *coder)
{
	t_dongle	*dongle;

	dongle = &sim->dongles[coder->left];
	pthread_mutex_lock(&sim->lock);
	coder->state = STATE_WAITING_DONGLES;
	while (!sim->stop && !is_free_and_cooled(dongle))
		pthread_cond_wait(&sim->event, &sim->lock);
	if (!sim->stop)
	{
		dongle->holder = coder->id;
		dongle->hands = 1;
		log_state(sim, coder->id, "has taken a dongle");
		while (!sim->stop)
			pthread_cond_wait(&sim->event, &sim->lock);
	}
	pthread_mutex_unlock(&sim->lock);
}

/* Enqueue both requests, then wait until the grants complete our pair. */
static void	wait_for_pair(t_sim *sim, t_coder *coder,
		t_dongle *left, t_dongle *right)
{
	t_request	request;

	request.coder_id = coder->id;
	request.deadline = coder->deadline;
	request.arrival_seq = sim->next_arrival++;
	heap_push(&left->waiting, sim->config.scheduler, &request);
	heap_push(&right->waiting, sim->config.scheduler, &request);
	while (!sim->stop && !coder_has_both_dongles(sim, coder))
	{
		grant_ready_pairs(sim);
		pthread_cond_broadcast(&sim->event);
		if (coder_has_both_dongles(sim, coder) || sim->stop)
			break ;
		pthread_cond_wait(&sim->event, &sim->lock);
	}
}

void	acquire_dongles(t_sim *sim, t_coder *coder)
{
	t_dongle	*left;
	t_dongle	*right;

	if (coder->left == coder->right)
	{
		acquire_single_dongle(sim, coder);
		return ;
	}
	left = &sim->dongles[coder->left];
	right = &sim->dongles[coder->right];
	pthread_mutex_lock(&sim->lock);
	coder->state = STATE_WAITING_DONGLES;
	wait_for_pair(sim, coder, left, right);
	pthread_mutex_unlock(&sim->lock);
}

/*
** Drop our still-queued requests from both heaps. Used when the
** simulation ends while we were waiting: the coder leaves the queues
** clean so no stale entry can ever block a grant.
*/
void	withdraw_requests(t_sim *sim, t_coder *coder)
{
	t_dongle	*dongle;

	dongle = &sim->dongles[coder->left];
	heap_remove_coder(&dongle->waiting, sim->config.scheduler, coder->id);
	dongle = &sim->dongles[coder->right];
	heap_remove_coder(&dongle->waiting, sim->config.scheduler, coder->id);
}
