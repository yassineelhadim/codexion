/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 00:00:00 by yel-hadi       #+#    #+#             */
/*   Updated: 2026/09/11 00:00:00 by yel-hadi      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>

#include "codexion.h"

/*
** The monitor is a separate thread with three duties:
**
**  1. burnout detection: a coder that has not STARTED a compile within
**     time_to_burnout ms since the previous compile start (or since the
**     simulation began) burns out. The log must land within 10 ms of the
**     deadline, so instead of polling blindly the monitor sleeps until
**     the earliest deadline among all living coders: detection is exact
**     to about a millisecond and costs no CPU in between.
**
**  2. completion detection: once every coder has compiled at least
**     number_of_compiles_required times, the simulation stops (success).
**
**  3. service ticks: broadcast every millisecond. That wakes coders
**     blocked on a dongle whose cooldown has just expired (a pure time
**     event that nobody else would signal) and lets queued grants go
**     through as soon as they become possible.
**
** Locking discipline: the loop holds sim->lock while inspecting state,
** and pthread_cond_timedwait releases it atomically while sleeping and
** reacquires it before returning. Whenever the simulation ends, stop is
** set BEFORE unlocking, and the broadcast happens right after.
*/

/*
** End the simulation because of a burnout. The "burned out" line must be
** printed even though stop is already set (log_state goes silent on stop
** so that nothing can appear AFTER the burnout), so it is printed here
** directly, while still holding the lock: the line stays atomic and is
** guaranteed to be the last one. Called with sim->lock held.
** Returns 1 when a burnout was declared.
*/
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
	sim->burnout_id = worst->id;
	sim->stop = 1;
	printf("%lld %d burned out\n", now - sim->start_time, worst->id);
	fflush(stdout);
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
		pthread_mutex_unlock(&sim->lock);
		pthread_cond_broadcast(&sim->event);
		return (0);
	}
	if (now >= *next_tick)
	{
		pthread_cond_broadcast(&sim->event);
		*next_tick = now + 1;
	}
	fill_wake_up_time(&wake_up, *next_tick);
	pthread_cond_timedwait(&sim->event, &sim->lock, &wake_up);
	pthread_mutex_unlock(&sim->lock);
	return (1);
}
