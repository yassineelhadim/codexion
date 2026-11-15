/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/09/07 22:47:09 by mberouak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_simulation_environment(t_env *env)
{
	free(env->dongle_states);
	free(env->last_compile_start_ms);
	free(env->completed_compiles);
	free(env->has_pending_request);
	free(env->priority_primary);
	free(env->priority_secondary);
	priority_heap_free(&env->request_heap);
	pthread_mutex_destroy(&env->state_mutex);
	pthread_mutex_destroy(&env->output_mutex);
	pthread_cond_destroy(&env->state_changed);
}
