/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 00:00:00 by yel-hadi       #+#    #+#             */
/*   Updated: 2026/09/11 00:00:00 by yel-hadi      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long long)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	log_state(t_sim *sim, int coder_id, const char *action)
{
	if (sim->stop)
		return ;
	printf("%lld %d %s\n", time_in_ms() - sim->start_time, coder_id, action);
}

void	sleep_interruptible(t_sim *sim, long duration_ms)
{
	struct timespec	deadline;
	long long		wake_at;
	int				timed_out;
	int				wait_result;

	wake_at = time_in_ms() + duration_ms;
	deadline.tv_sec = (time_t)(wake_at / 1000);
	deadline.tv_nsec = (long)((wake_at % 1000) * 1000000L);
	timed_out = 0;
	pthread_mutex_lock(&sim->lock);
	while (!sim->stop && !timed_out)
	{
		wait_result = pthread_cond_timedwait(&sim->event, &sim->lock,
				&deadline);
		if (wait_result != 0)
			timed_out = 1;
	}
	pthread_mutex_unlock(&sim->lock);
}
