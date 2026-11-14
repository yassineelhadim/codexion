/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setters.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:34:48 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:34:49 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	broadcast_shutdown(t_config *config)
{
	int	i;

	safe_mutex_handle(&config->start_mutex, LOCK);
	safe_cond_handle(&config->start_cond, BROADCAST);
	safe_mutex_handle(&config->start_mutex, UNLOCK);
	i = 0;
	while (i < config->coder_count)
	{
		safe_mutex_handle(&config->dongles[i].mutex, LOCK);
		safe_cond_handle(&config->dongles[i].cond, BROADCAST);
		safe_mutex_handle(&config->dongles[i].mutex, UNLOCK);
		i++;
	}
}

void	set_sim_state(t_config *config, bool state)
{
	safe_mutex_handle(&config->monitor->mutex, LOCK);
	config->monitor->running = state;
	safe_mutex_handle(&config->monitor->mutex, UNLOCK);
	if (!state)
	{
		broadcast_shutdown(config);
	}
}

void	increment_compile_count(t_coder *coder)
{
	safe_mutex_handle(&coder->mutex, LOCK);
	coder->compile_count++;
	safe_mutex_handle(&coder->mutex, UNLOCK);
}

void	set_last_compile_start(t_coder *coder, long long time)
{
	safe_mutex_handle(&coder->mutex, LOCK);
	coder->last_compile_start = time;
	safe_mutex_handle(&coder->mutex, UNLOCK);
}
