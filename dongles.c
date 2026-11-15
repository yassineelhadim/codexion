/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/11/15 18:08:30 by mberouak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	grant_top_request(t_sim *sim, long long now)
{
	t_request	top;
	int			right;
	int			left;

	top = queue_pop(&sim->queue);
	right = top.coder_id;
	left = (top.coder_id - 1 + sim->coders_count) % sim->coders_count;
	if (right == left || sim->dongles[right].in_use
		|| sim->dongles[left].in_use
		|| now < sim->dongles[right].ready_at_ms
		|| now < sim->dongles[left].ready_at_ms)
		return (0);
	sim->dongles[right].in_use = 1;
	sim->dongles[left].in_use = 1;
	sim->is_waiting[top.coder_id] = 0;
	sim->last_compile_ms[top.coder_id] = now;
	log_status_locked(sim, top.coder_id, "has taken a dongle");
	log_status_locked(sim, top.coder_id, "has taken a dongle");
	log_status_locked(sim, top.coder_id, "is compiling");
	return (1);
}

static void	process_queue(t_sim *sim)
{
	long long	now;
	int			id;
	int			granted;

	if (sim->stopped)
		return ;
	now = now_ms(sim);
	sim->queue.size = 0;
	id = 0;
	while (id < sim->coders_count)
	{
		if (sim->is_waiting[id])
			queue_push(&sim->queue, sim->req_priority[id],
				sim->req_order[id], id);
		id++;
	}
	granted = 0;
	while (!queue_empty(&sim->queue))
	{
		if (grant_top_request(sim, now))
			granted = 1;
	}
	if (granted)
		pthread_cond_broadcast(&sim->wake_signal);
}

static int	wait_turn(t_sim *sim, int coder_id)
{
	if (sim->stopped)
	{
		sim->is_waiting[coder_id] = 0;
		return (0);
	}
	if (!sim->is_waiting[coder_id])
		return (1);
	pthread_mutex_unlock(&sim->lock);
	usleep(1000);
	pthread_mutex_lock(&sim->lock);
	process_queue(sim);
	return (-1);
}

int	take_dongles(t_sim *sim, int coder_id)
{
	long long	ticket;
	int			status;

	pthread_mutex_lock(&sim->lock);
	ticket = (sim->next_ticket)++;
	if (sim->mode == MODE_FIFO)
		sim->req_priority[coder_id] = ticket;
	else
		sim->req_priority[coder_id] = sim->last_compile_ms[coder_id]
			+ sim->burnout_ms;
	sim->req_order[coder_id] = ticket;
	sim->is_waiting[coder_id] = 1;
	process_queue(sim);
	while (1)
	{
		status = wait_turn(sim, coder_id);
		if (status != -1)
		{
			pthread_mutex_unlock(&sim->lock);
			return (status);
		}
	}
}

void	drop_dongles(t_sim *sim, int coder_id)
{
	int			right;
	int			left;
	long long	now;

	right = coder_id;
	left = (coder_id - 1 + sim->coders_count) % sim->coders_count;
	pthread_mutex_lock(&sim->lock);
	now = now_ms(sim);
	sim->dongles[right].in_use = 0;
	sim->dongles[right].ready_at_ms = now + sim->cooldown_ms;
	if (left != right)
	{
		sim->dongles[left].in_use = 0;
		sim->dongles[left].ready_at_ms = now + sim->cooldown_ms;
	}
	process_queue(sim);
	pthread_cond_broadcast(&sim->wake_signal);
	pthread_mutex_unlock(&sim->lock);
}
