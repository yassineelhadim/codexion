/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:33:40 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:33:41 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dongle_utils.h"
#include "safe_wrappers_utils.h"

void	init_request(t_request *request, t_coder *coder)
{
	long long	last;

	last = get_last_compile_start(coder);
	request->coder_id = coder->id;
	request->timestamp = get_time_us();
	if (last == 0)
		request->deadline = coder->config->started_at
			+ coder->config->time_to_burnout;
	else
		request->deadline = last + coder->config->time_to_burnout;
}

void	get_tv(struct timespec *ts, long long target_us)
{
	ts->tv_sec = target_us / 1000000;
	ts->tv_nsec = (target_us % 1000000) * 1000;
}

bool	can_acquire(t_coder *coder, t_dongle *d)
{
	return (heap_peek(d).coder_id == coder->id
		&& d->held_by == -1
		&& get_time_us() >= d->available_at);
}

t_status	wait_for_dongle(t_coder *coder, t_dongle *d)
{
	struct timespec	ts;

	if (!get_sim_state(coder->config))
		return (FAILURE);
	if (get_time_us() < d->available_at)
	{
		get_tv(&ts, d->available_at);
		safe_cond_timedwait(&d->cond, &d->mutex, &ts);
	}
	else
	{
		safe_cond_wait(&d->cond, &d->mutex);
	}
	return (SUCCESS);
}
