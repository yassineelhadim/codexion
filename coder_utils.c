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

#include <errno.h>

#include "codexion.h"

/*
** Wall-clock time in milliseconds since the Epoch, as recommended by the
** subject (gettimeofday).
*/
long long	time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long long)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

/*
** Print one state change. The caller must hold sim->lock, which makes the
** whole printf atomic with respect to every other log line: two messages
** can never interleave on the same line. Once the simulation is over the
** log falls silent, so nothing can be printed after a burnout or the
** final completion.
*/
void	log_state(t_sim *sim, int coder_id, const char *action)
{
	if (sim->stop)
		return ;
	printf("%lld %d %s\n", time_in_ms() - sim->start_time, coder_id, action);
	fflush(stdout);
}

/*
** Sleep for duration_ms, but wake up immediately if the simulation ends
** (burnout or completion). usleep() alone would keep every thread running
** for up to its whole sleep duration after the stop flag is set, which
** would break the 10 ms burnout-report deadline and slow down shutdown.
** Instead we sleep on the shared condition variable with an absolute
** timeout - the standard interruptible-sleep pattern:
**
**   lock; while (!stop && !timed_out) cond_timedwait(...); unlock
**
** The function takes the lock itself: callers must NOT hold sim->lock
** when calling it, and it returns with the lock released.
** The spurious-wakeup loop is required by POSIX: cond_timedwait may
** return without the condition being signalled.
*/
void	sleep_interruptible(t_sim *sim, long duration_ms)
{
	struct timespec	deadline;
	long long		wake_at;
	int				timed_out;

	wake_at = time_in_ms() + duration_ms;
	deadline.tv_sec = (time_t)(wake_at / 1000);
	deadline.tv_nsec = (long)((wake_at % 1000) * 1000000L);
	timed_out = 0;
	pthread_mutex_lock(&sim->lock);
	while (!sim->stop && !timed_out)
	{
		if (pthread_cond_timedwait(&sim->event, &sim->lock, &deadline)
			== ETIMEDOUT)
			timed_out = 1;
	}
	pthread_mutex_unlock(&sim->lock);
}
