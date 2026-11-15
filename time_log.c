/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_log.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/11/15 18:08:30 by mberouak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	now_ms(t_sim *sim)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec - sim->start_time.tv_sec) * 1000LL
		+ (tv.tv_usec - sim->start_time.tv_usec) / 1000LL);
}

void	set_timeout(struct timespec *ts, t_sim *sim, long long ms)
{
	long long	usec;

	usec = (long long)sim->start_time.tv_sec * 1000000LL
		+ sim->start_time.tv_usec + ms * 1000LL;
	ts->tv_sec = usec / 1000000LL;
	ts->tv_nsec = (usec % 1000000LL) * 1000LL;
}

void	log_status(t_sim *sim, int coder_id, const char *msg)
{
	pthread_mutex_lock(&sim->lock);
	if (!sim->stopped)
		log_status_locked(sim, coder_id, msg);
	pthread_mutex_unlock(&sim->lock);
}

void	log_status_locked(t_sim *sim, int coder_id, const char *msg)
{
	pthread_mutex_lock(&sim->print_lock);
	printf("%lld %d %s\n", now_ms(sim), coder_id + 1, msg);
	pthread_mutex_unlock(&sim->print_lock);
}

int	is_stopped(t_sim *sim)
{
	int	stop;

	pthread_mutex_lock(&sim->lock);
	stop = sim->stopped;
	pthread_mutex_unlock(&sim->lock);
	return (stop);
}
