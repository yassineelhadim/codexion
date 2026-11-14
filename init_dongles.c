/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:34:09 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:34:10 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "init_dongles.h"
#include "init_utils.h"

static t_status	init_dongle_sync(t_dongle *dongle)
{
	if (safe_mutex_handle(&dongle->mutex, INIT) == FAILURE)
		return (FAILURE);
	if (safe_cond_handle(&dongle->cond, INIT) == FAILURE)
	{
		safe_mutex_handle(&dongle->mutex, DESTROY);
		return (FAILURE);
	}
	return (SUCCESS);
}

static t_status	init_dongle_queue(t_dongle *dongle, int capacity)
{
	dongle->queue = malloc(sizeof(t_request) * capacity);
	if (!dongle->queue)
	{
		safe_cond_handle(&dongle->cond, DESTROY);
		safe_mutex_handle(&dongle->mutex, DESTROY);
		return (FAILURE);
	}
	return (SUCCESS);
}

static t_status	init_single_dongle(t_dongle *dongle, int id,
									int capacity, t_config *config)
{
	if (init_dongle_sync(dongle) == FAILURE)
		return (FAILURE);
	if (init_dongle_queue(dongle, capacity) == FAILURE)
		return (FAILURE);
	set_dongle_basic_fields(dongle, id, capacity);
	set_dongle_comparator(dongle, config);
	return (SUCCESS);
}

t_status	init_dongles(t_config *config)
{
	int	i;

	i = 0;
	while (i < config->coder_count)
	{
		if (init_single_dongle(&config->dongles[i], i,
				2, config) == FAILURE)
			return (FAILURE);
		config->dongles_init_count = i + 1;
		i++;
	}
	return (SUCCESS);
}
