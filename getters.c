/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getters.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:33:52 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:33:53 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	get_sim_state(t_config *config)
{
	bool	state;

	safe_mutex_handle(&config->monitor->mutex, LOCK);
	state = config->monitor->running;
	safe_mutex_handle(&config->monitor->mutex, UNLOCK);
	return (state);
}

int	get_compile_count(t_coder *coder)
{
	int	compile_count;

	safe_mutex_handle(&coder->mutex, LOCK);
	compile_count = coder->compile_count;
	safe_mutex_handle(&coder->mutex, UNLOCK);
	return (compile_count);
}

long long	get_last_compile_start(t_coder *coder)
{
	long long	last_compile;

	safe_mutex_handle(&coder->mutex, LOCK);
	last_compile = coder->last_compile_start;
	safe_mutex_handle(&coder->mutex, UNLOCK);
	return (last_compile);
}
