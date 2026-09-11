/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 00:00:00 by yel-hadi       #+#    #+#             */
/*   Updated: 2026/09/11 00:00:00 by yel-hadi      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** One thread per coder. The cycle is:
**
**   wait for both dongles -> compile (holding them) -> release dongles
**   -> debug -> refactor -> back to waiting for dongles
**
** The burnout deadline is refreshed at every compile START, as required:
** deadline = last_compile_start + time_to_burnout. The value that was
** current when the requests were queued is what the EDF arbitration sees.
**
** Every phase sleeps interruptibly: if the monitor ends the simulation
** anywhere, all coders wake up immediately, see sim->stop and exit
** cleanly (pending queue requests are withdrawn under the lock).
*/

static int	simulation_is_over(t_sim *sim)
{
	int	stopped;

	pthread_mutex_lock(&sim->lock);
	stopped = sim->stop;
	pthread_mutex_unlock(&sim->lock);
	return (stopped);
}

static void	compile_phase(t_sim *sim, t_coder *coder)
{
	acquire_dongles(sim, coder);
	pthread_mutex_lock(&sim->lock);
	if (sim->stop)
	{
		withdraw_requests(sim, coder);
		pthread_mutex_unlock(&sim->lock);
		return ;
	}
	coder->state = STATE_COMPILING;
	coder->deadline = time_in_ms() + sim->config.time_to_burnout;
	log_state(sim, coder->id, "is compiling");
	pthread_mutex_unlock(&sim->lock);
	sleep_interruptible(sim, sim->config.time_to_compile);
	pthread_mutex_lock(&sim->lock);
	if (!sim->stop)
		coder->nb_compiles++;
	pthread_mutex_unlock(&sim->lock);
	release_dongles(sim, coder);
}

static int	debug_refactor_phase(t_sim *sim, t_coder *coder)
{
	pthread_mutex_lock(&sim->lock);
	if (sim->stop)
	{
		pthread_mutex_unlock(&sim->lock);
		return (0);
	}
	coder->state = STATE_DEBUGGING;
	log_state(sim, coder->id, "is debugging");
	pthread_mutex_unlock(&sim->lock);
	sleep_interruptible(sim, sim->config.time_to_debug);
	pthread_mutex_lock(&sim->lock);
	if (sim->stop)
	{
		pthread_mutex_unlock(&sim->lock);
		return (0);
	}
	coder->state = STATE_REFACTORING;
	log_state(sim, coder->id, "is refactoring");
	pthread_mutex_unlock(&sim->lock);
	sleep_interruptible(sim, sim->config.time_to_refactor);
	return (1);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_sim	*sim;

	coder = (t_coder *)arg;
	sim = coder->sim;
	while (!simulation_is_over(sim))
	{
		compile_phase(sim, coder);
		if (simulation_is_over(sim))
			break ;
		if (!debug_refactor_phase(sim, coder))
			break ;
	}
	return (NULL);
}
