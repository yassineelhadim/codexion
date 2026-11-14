/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:27:33 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:33:24 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	cleanup_dongles(t_config *config)
{
	int			i;
	t_dongle	*dongle;

	if (!config->dongles_allocated || !config->dongles)
		return ;
	i = 0;
	while (i < config->dongles_init_count)
	{
		dongle = &config->dongles[i];
		safe_mutex_handle(&dongle->mutex, DESTROY);
		safe_cond_handle(&dongle->cond, DESTROY);
		if (dongle->queue)
		{
			free(dongle->queue);
			dongle->queue = NULL;
		}
		i++;
	}
	free(config->dongles);
	config->dongles = NULL;
	config->dongles_allocated = false;
}

static void	cleanup_coders(t_config *config)
{
	int	i;

	if (!config->coders_allocated || !config->coders)
		return ;
	i = 0;
	while (i < config->coders_init_count)
	{
		safe_mutex_handle(&config->coders[i].mutex, DESTROY);
		i++;
	}
	free(config->coders);
	config->coders = NULL;
	config->coders_allocated = false;
}

static void	cleanup_monitor(t_config *config)
{
	if (!config->monitor_allocated || !config->monitor)
		return ;
	safe_mutex_handle(&config->monitor->mutex, DESTROY);
	free(config->monitor);
	config->monitor = NULL;
	config->monitor_allocated = false;
}

static void	cleanup_global_sync(t_config *config)
{
	if (config->event_mutex_init)
	{
		safe_mutex_handle(&config->event_mutex, DESTROY);
		config->event_mutex_init = false;
	}
	if (config->start_mutex_init)
	{
		safe_mutex_handle(&config->start_mutex, DESTROY);
		config->start_mutex_init = false;
	}
	if (config->start_cond_init)
	{
		safe_cond_handle(&config->start_cond, DESTROY);
		config->start_cond_init = false;
	}
}

void	cleanup(t_config *config)
{
	if (!config)
		return ;
	cleanup_monitor(config);
	cleanup_coders(config);
	cleanup_dongles(config);
	cleanup_global_sync(config);
}
