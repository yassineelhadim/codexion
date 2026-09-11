/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_launch.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 00:00:00 by yel-hadi       #+#    #+#             */
/*   Updated: 2026/09/11 00:00:00 by yel-hadi      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Thread orchestration:
**   - spawn one thread per coder, then the monitor thread;
**   - wait for everyone to finish.
**
** Rollback rule: if any pthread_create fails, we set sim->stop, wake
** every thread (so nobody stays blocked on a dongle), join what was
** already created, and report failure. Nothing is left running.
*/

static void	stop_simulation(t_sim *sim)
{
	pthread_mutex_lock(&sim->lock);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->lock);
	pthread_cond_broadcast(&sim->event);
}

static int	spawn_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.nb_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine,
				&sim->coders[i]) != 0)
		{
			stop_simulation(sim);
			while (--i >= 0)
				pthread_join(sim->coders[i].thread, NULL);
			return (1);
		}
		i++;
	}
	return (0);
}

/* Join the coders only; used when the monitor thread was never created. */
static void	join_everyone_without_monitor(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.nb_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

static void	join_everyone(t_sim *sim, pthread_t monitor)
{
	int	i;

	i = 0;
	while (i < sim->config.nb_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	pthread_join(monitor, NULL);
}

/*
** Run the simulation to its end (burnout or completion) and return 0,
** or return 1 if a thread could not be created.
*/
int	sim_run(t_sim *sim)
{
	pthread_t	monitor;

	if (spawn_coders(sim) != 0)
		return (1);
	if (pthread_create(&monitor, NULL, monitor_routine, sim) != 0)
	{
		stop_simulation(sim);
		join_everyone_without_monitor(sim);
		return (1);
	}
	join_everyone(sim, monitor);
	return (0);
}
