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

void	env_destroy(t_env *env)
{
	free(env->dongles);
	free(env->last_start);
	free(env->done);
	free(env->waiting);
	free(env->k1);
	free(env->k2);
	heap_free(&env->work);
	pthread_mutex_destroy(&env->lock);
	pthread_mutex_destroy(&env->log_lock);
	pthread_cond_destroy(&env->cond);
}
