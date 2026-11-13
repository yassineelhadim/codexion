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

static int	log_burnout_if_due(t_sim *sim, long long now)
{
	t_coder	*worst;
	int		i;

	worst = NULL;
	i = 0;
	while (i < sim->config.nb_coders)
	{
		if (sim->coders[i].state != STATE_BURNED_OUT
			&& now >= sim->coders[i].deadline
			&& (worst == NULL
				|| sim->coders[i].deadline < worst->deadline))
			worst = &sim->coders[i];
		i++;
	}
	if (worst == NULL)
		return (0);
	worst->state = STATE_BURNED_OUT;
	sim->stop = 1;
	printf("%lld %d burned out\n", now - sim->start_time, worst->id);
	return (1);
}

static int	all_compiles_done(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.nb_coders)
	{
		if (sim->coders[i].nb_compiles < sim->config.nb_compiles_required)
			return (0);
		i++;
	}
	return (1);
}

static void	fill_wake_up_time(struct timespec *wake_up, long long at_ms)
{
	wake_up->tv_sec = (time_t)(at_ms / 1000);
	wake_up->tv_nsec = (long)((at_ms % 1000) * 1000000L);
}

static int	monitor_step(t_sim *sim, long long *next_tick);

void	*monitor_routine(void *arg)
{
	t_sim			*sim;
	long long		next_tick;

	sim = (t_sim *)arg;
	next_tick = time_in_ms();
	while (monitor_step(sim, &next_tick))
		;
	return (NULL);
}

static int	monitor_step(t_sim *sim, long long *next_tick)
{
	struct timespec	wake_up;
	long long		now;

	pthread_mutex_lock(&sim->lock);
	now = time_in_ms();
	if (all_compiles_done(sim) || log_burnout_if_due(sim, now))
	{
		sim->stop = 1;
		pthread_cond_broadcast(&sim->event);
		pthread_mutex_unlock(&sim->lock);
		return (0);
	}
	if (now >= *next_tick)
	{
		grant_ready_pairs(sim);
		pthread_cond_broadcast(&sim->event);
		*next_tick = now + 1;
	}
	fill_wake_up_time(&wake_up, *next_tick);
	pthread_cond_timedwait(&sim->event, &sim->lock, &wake_up);
	pthread_mutex_unlock(&sim->lock);
	return (1);
}
