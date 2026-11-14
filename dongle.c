/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:33:47 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:33:48 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include "dongle_utils.h"

static t_status	try_acquire(t_coder *coder, t_dongle *d)
{
	while (get_sim_state(coder->config))
	{
		if (can_acquire(coder, d))
		{
			heap_pop(d);
			d->held_by = coder->id;
			safe_mutex_handle(&d->mutex, UNLOCK);
			return (SUCCESS);
		}
		if (wait_for_dongle(coder, d) == FAILURE)
			break ;
	}
	heap_remove(d, coder->id);
	safe_mutex_handle(&d->mutex, UNLOCK);
	return (FAILURE);
}

t_status	acquire_dongle(t_coder *coder, t_dongle *d)
{
	t_request	request;

	safe_mutex_handle(&d->mutex, LOCK);
	if (!get_sim_state(coder->config))
	{
		safe_mutex_handle(&d->mutex, UNLOCK);
		return (FAILURE);
	}
	init_request(&request, coder);
	if (!(get_sim_state(coder->config)))
	{
		safe_mutex_handle(&d->mutex, UNLOCK);
		return (FAILURE);
	}
	heap_push(d, request);
	return (try_acquire(coder, d));
}

void	release_dongle(t_coder *coder, t_dongle *d)
{
	safe_mutex_handle(&d->mutex, LOCK);
	d->available_at = get_time_us() + coder->config->dongle_cooldown;
	d->held_by = -1;
	safe_cond_handle(&d->cond, BROADCAST);
	safe_mutex_handle(&d->mutex, UNLOCK);
}

void	release_two_dongles(t_coder *coder)
{
	release_dongle(coder, coder->second_dongle);
	release_dongle(coder, coder->first_dongle);
}

t_status	acquire_two_dongles(t_coder *coder)
{
	if (acquire_dongle(coder, coder->first_dongle) == FAILURE)
		return (FAILURE);
	event(coder->config, coder, "has taken a dongle");
	if (acquire_dongle(coder, coder->second_dongle) == FAILURE)
	{
		release_dongle(coder, coder->first_dongle);
		return (FAILURE);
	}
	event(coder->config, coder, "has taken a dongle");
	return (SUCCESS);
}
