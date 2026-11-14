/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   console.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:33:37 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:33:38 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_status	event(t_config *config, t_coder *coder, const char *msg)
{
	long long	timestamp;

	safe_mutex_handle(&config->event_mutex, LOCK);
	if (!get_sim_state(config))
	{
		safe_mutex_handle(&config->event_mutex, UNLOCK);
		return (FAILURE);
	}
	timestamp = (get_time_us() - config->started_at) / 1000;
	printf("%lld %d %s\n", timestamp, coder->id, msg);
	safe_mutex_handle(&config->event_mutex, UNLOCK);
	return (SUCCESS);
}

t_status	event_burnout(t_config *config, t_coder *coder)
{
	long long	timestamp;

	safe_mutex_handle(&config->event_mutex, LOCK);
	timestamp = (get_time_us() - config->started_at) / 1000;
	printf("%lld %d %s\n", timestamp, coder->id, "burned out");
	safe_mutex_handle(&config->event_mutex, UNLOCK);
	return (SUCCESS);
}
