/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:34:51 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:34:52 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static t_status	create_coder_threads(t_config *config)
{
	int	i;

	i = 0;
	while (i < config->coder_count)
	{
		if (safe_thread_create(&config->coders[i].thread,
				coder_routine, &config->coders[i]) == FAILURE)
			return (FAILURE);
		i++;
	}
	return (SUCCESS);
}

static void	join_coder_threads(t_config *config)
{
	int	i;

	i = 0;
	while (i < config->coder_count)
	{
		safe_thread_join(config->coders[i].thread);
		i++;
	}
}

t_status	simulate(t_config *config)
{
	if (safe_thread_create(&config->monitor->thread,
			monitor_routine, config) == FAILURE)
		return (FAILURE);
	if (create_coder_threads(config) == FAILURE)
	{
		return (FAILURE);
	}
	safe_mutex_handle(&config->start_mutex, LOCK);
	config->started_at = get_time_us();
	config->sim_started = true;
	safe_cond_handle(&config->start_cond, BROADCAST);
	safe_mutex_handle(&config->start_mutex, UNLOCK);
	safe_thread_join(config->monitor->thread);
	join_coder_threads(config);
	return (SUCCESS);
}
