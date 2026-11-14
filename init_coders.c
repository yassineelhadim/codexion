/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_coders.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:34:03 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:34:04 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "init_coders.h"
#include "init_utils.h"
#include "init_priority_utils.h"

static t_status	init_single_coder(t_coder *coder, int id, t_config *config)
{
	coder->id = id + 1;
	if (safe_mutex_handle(&coder->mutex, INIT) == FAILURE)
		return (FAILURE);
	coder->last_compile_start = 0;
	coder->compile_count = 0;
	assign_coder_dongles(coder, id, config);
	set_coder_dongle_priority(coder, config);
	coder->config = config;
	return (SUCCESS);
}

t_status	init_coders(t_config *config)
{
	int	i;

	i = 0;
	while (i < config->coder_count)
	{
		if (init_single_coder(&config->coders[i], i, config) == FAILURE)
			return (FAILURE);
		config->coders_init_count = i + 1;
		i++;
	}
	return (SUCCESS);
}

t_status	init_monitor(t_config *config)
{
	if (safe_mutex_handle(&config->monitor->mutex, INIT) == FAILURE)
		return (FAILURE);
	config->monitor->running = true;
	return (SUCCESS);
}
