/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:34:30 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:34:31 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include "safe_wrappers_utils.h"

static void	wait_for_start(t_config *config)
{
	safe_mutex_handle(&config->start_mutex, LOCK);
	while (!config->sim_started)
		safe_cond_wait(&config->start_cond, &config->start_mutex);
	safe_mutex_handle(&config->start_mutex, UNLOCK);
}

static bool	check_burnout(t_config *config, t_coder *coder)
{
	long long	last;

	last = get_last_compile_start(coder);
	if (last == 0)
		last = config->started_at;
	if (last + config->time_to_burnout <= get_time_us())
	{
		set_sim_state(config, false);
		event_burnout(config, coder);
		return (true);
	}
	return (false);
}

static bool	check_all_coders(t_config *config)
{
	int	i;
	int	coders_done;

	i = 0;
	coders_done = 0;
	while (i < config->coder_count)
	{
		if (get_compile_count(&config->coders[i])
			== config->compiles_required)
		{
			coders_done++;
			i++;
			continue ;
		}
		if (check_burnout(config, &config->coders[i]))
			return (true);
		i++;
	}
	if (coders_done == config->coder_count)
	{
		set_sim_state(config, false);
		return (true);
	}
	return (false);
}

void	*monitor_routine(void *arg)
{
	t_config	*config;

	config = arg;
	wait_for_start(config);
	while (get_sim_state(config))
	{
		if (check_all_coders(config))
			return (NULL);
		nap(config, 1000);
	}
	return (NULL);
}
