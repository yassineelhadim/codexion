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
