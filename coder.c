/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/11/15 18:08:30 by mberouak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	all_done(t_sim *sim)
{
	int	id;

	id = 0;
	while (id < sim->coders_count)
	{
		if (sim->compiles_done[id] < sim->goal_compiles)
			return (0);
		id++;
	}
	return (1);
}

static int	do_compile(t_sim *sim, int coder_id)
{
	int	stop;

	smart_sleep(sim, sim->compile_ms);
	drop_dongles(sim, coder_id);
	pthread_mutex_lock(&sim->lock);
	sim->compiles_done[coder_id]++;
	if (all_done(sim))
	{
		sim->stopped = 1;
		pthread_cond_broadcast(&sim->wake_signal);
	}
	stop = sim->stopped;
	pthread_mutex_unlock(&sim->lock);
	return (!stop);
}

static int	do_phase(t_sim *sim, int coder_id, const char *msg, long long ms)
{
	pthread_mutex_lock(&sim->lock);
	if (sim->stopped)
	{
		pthread_mutex_unlock(&sim->lock);
		return (0);
	}
	log_status_locked(sim, coder_id, msg);
	pthread_mutex_unlock(&sim->lock);
	smart_sleep(sim, ms);
	return (!is_stopped(sim));
}

void	*coder_loop(void *arg)
{
	t_coder_arg	*coder_arg;
	t_sim		*sim;
	int			coder_id;

	coder_arg = arg;
	sim = coder_arg->sim;
	coder_id = coder_arg->coder_id;
	free(coder_arg);
	while (1)
	{
		if (!take_dongles(sim, coder_id))
			break ;
		if (!do_compile(sim, coder_id))
			break ;
		if (!do_phase(sim, coder_id, "is debugging", sim->debug_ms))
			break ;
		if (!do_phase(sim, coder_id, "is refactoring", sim->refactor_ms))
			break ;
	}
	return (NULL);
}
