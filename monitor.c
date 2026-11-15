/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/11/15 18:08:30 by mberouak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long long	next_burnout(t_sim *sim, int *who)
{
	long long	earliest;
	long long	deadline;
	int			id;

	earliest = LLONG_MAX;
	*who = -1;
	id = 0;
	while (id < sim->coders_count)
	{
		deadline = sim->last_compile_ms[id] + sim->burnout_ms;
		if (deadline < earliest)
		{
			earliest = deadline;
			*who = id;
		}
		id++;
	}
	return (earliest);
}

void	*monitor_loop(void *arg)
{
	t_sim		*sim;
	long long	deadline;
	int			who;

	sim = arg;
	pthread_mutex_lock(&sim->lock);
	while (!sim->stopped)
	{
		deadline = next_burnout(sim, &who);
		if (now_ms(sim) >= deadline)
		{
			sim->stopped = 1;
			log_status_locked(sim, who, "burned out");
			pthread_cond_broadcast(&sim->wake_signal);
			break ;
		}
		pthread_mutex_unlock(&sim->lock);
		usleep(1000);
		pthread_mutex_lock(&sim->lock);
	}
	pthread_mutex_unlock(&sim->lock);
	return (NULL);
}
