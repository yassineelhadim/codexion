/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:34:24 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:34:25 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include "init_dongles.h"
#include "init_coders.h"

static t_status	init_global_sync(t_config *config)
{
	if (safe_mutex_handle(&config->event_mutex, INIT) == FAILURE)
		return (FAILURE);
	config->event_mutex_init = true;
	if (safe_mutex_handle(&config->start_mutex, INIT) == FAILURE)
		return (FAILURE);
	config->start_mutex_init = true;
	if (safe_cond_handle(&config->start_cond, INIT) == FAILURE)
		return (FAILURE);
	config->start_cond_init = true;
	return (SUCCESS);
}

static t_status	allocate_coders(t_config *config)
{
	config->coders = malloc(sizeof(t_coder) * config->coder_count);
	if (!config->coders)
		return (FAILURE);
	memset(config->coders, 0, sizeof(t_coder) * config->coder_count);
	config->coders_allocated = true;
	return (SUCCESS);
}

static t_status	allocate_dongles(t_config *config)
{
	config->dongles = malloc(sizeof(t_dongle) * config->coder_count);
	if (!config->dongles)
		return (FAILURE);
	memset(config->dongles, 0, sizeof(t_dongle) * config->coder_count);
	config->dongles_allocated = true;
	return (SUCCESS);
}

static t_status	allocate_monitor(t_config *config)
{
	config->monitor = malloc(sizeof(t_monitor));
	if (!config->monitor)
		return (FAILURE);
	memset(config->monitor, 0, sizeof(t_monitor));
	config->monitor_allocated = true;
	return (SUCCESS);
}

t_status	init_config(t_config *config)
{
	if (init_global_sync(config) == FAILURE)
		return (FAILURE);
	if (allocate_coders(config) == FAILURE)
		return (FAILURE);
	if (allocate_dongles(config) == FAILURE)
		return (FAILURE);
	if (allocate_monitor(config) == FAILURE)
		return (FAILURE);
	if (init_dongles(config) == FAILURE)
		return (FAILURE);
	if (init_coders(config) == FAILURE)
		return (FAILURE);
	if (init_monitor(config) == FAILURE)
		return (FAILURE);
	config->started_at = 0;
	return (SUCCESS);
}
