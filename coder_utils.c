/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:28:17 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:33:27 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder_utils.h"

t_status	compile(t_coder *coder)
{
	if (acquire_two_dongles(coder) == FAILURE)
		return (FAILURE);
	set_last_compile_start(coder, get_time_us());
	if (event(coder->config, coder, "is compiling") == FAILURE)
	{
		release_two_dongles(coder);
		return (FAILURE);
	}
	nap(coder->config, coder->config->time_to_compile);
	if (!(get_sim_state(coder->config)))
	{
		release_two_dongles(coder);
		return (FAILURE);
	}
	increment_compile_count(coder);
	release_two_dongles(coder);
	return (SUCCESS);
}

t_status	debug(t_coder *coder)
{
	if (event(coder->config, coder, "is debugging") == FAILURE)
		return (FAILURE);
	nap(coder->config, coder->config->time_to_debug);
	if (!(get_sim_state(coder->config)))
		return (FAILURE);
	return (SUCCESS);
}

t_status	refractor(t_coder *coder)
{
	if (event(coder->config, coder, "is refactoring") == FAILURE)
		return (FAILURE);
	nap(coder->config, coder->config->time_to_refactor);
	if (!(get_sim_state(coder->config)))
		return (FAILURE);
	return (SUCCESS);
}

void	one_coder(t_coder *coder)
{
	acquire_dongle(coder, coder->first_dongle);
	event(coder->config, coder, "has taken a dongle");
	release_dongle(coder, coder->first_dongle);
}
