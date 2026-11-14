/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:32:29 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/12 21:10:16 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include "coder_utils.h"
#include "safe_wrappers_utils.h"

static void	wait_for_start(t_coder *coder)
{
	safe_mutex_handle(&coder->config->start_mutex, LOCK);
	while (!coder->config->sim_started)
		safe_cond_wait(&coder->config->start_cond,
			&coder->config->start_mutex);
	if (!get_sim_state(coder->config))
	{
		safe_mutex_handle(&coder->config->start_mutex, UNLOCK);
		return ;
	}
	safe_mutex_handle(&coder->config->start_mutex, UNLOCK);
}

static void	*run_coder_loop(t_coder *coder)
{
	while (get_sim_state(coder->config))
	{
		if (get_compile_count(coder) >= coder->config->compiles_required)
			break ;
		if (compile(coder) == FAILURE)
			return (NULL);
		if (debug(coder) == FAILURE)
			return (NULL);
		if (refractor(coder) == FAILURE)
			return (NULL);
		if (get_compile_count(coder) == coder->config->compiles_required)
			return (NULL);
	}
	return (NULL);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = arg;
	wait_for_start(coder);
	if (!get_sim_state(coder->config))
		return (NULL);
	if (coder->config->coder_count == 1)
		return (one_coder(coder), (NULL));
	if (coder->id % 2 == 0)
		usleep(500);
	return (run_coder_loop(coder));
}
