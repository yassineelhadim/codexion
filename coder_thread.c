/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/09/07 22:47:09 by mberouak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	all_coders_complete(t_env *env)
{
	int	i;

	i = 0;
	while (i < env->coder_count)
	{
		if (env->completed_compiles[i] < env->required_compile_count)
			return (0);
		i++;
	}
	return (1);
}

static int	complete_compile_phase(t_env *env, int id)
{
	int	stop;

	sleep_milliseconds(env, env->compile_duration_ms);
	release_dongle_pair(env, id);
	pthread_mutex_lock(&env->state_mutex);
	env->completed_compiles[id]++;
	if (all_coders_complete(env))
	{
		env->simulation_stopped = 1;
		pthread_cond_broadcast(&env->state_changed);
	}
	stop = env->simulation_stopped;
	pthread_mutex_unlock(&env->state_mutex);
	return (!stop);
}

static int	perform_coder_activity(t_env *env, int id, const char *msg,
		long long duration)
{
	pthread_mutex_lock(&env->state_mutex);
	if (env->simulation_stopped)
	{
		pthread_mutex_unlock(&env->state_mutex);
		return (0);
	}
	write_state_locked(env, id, msg);
	pthread_mutex_unlock(&env->state_mutex);
	sleep_milliseconds(env, duration);
	return (!simulation_stopped(env));
}

void	*run_coder_thread(void *arg)
{
	t_ctx	*c;
	t_env	*env;
	int		id;

	c = arg;
	env = c->env;
	id = c->coder_index;
	free(c);
	while (1)
	{
		if (!request_dongle_pair(env, id))
			break ;
		if (!complete_compile_phase(env, id))
			break ;
		if (!perform_coder_activity(env, id, "is debugging",
				env->debug_duration_ms))
			break ;
		if (!perform_coder_activity(env, id, "is refactoring",
				env->refactor_duration_ms))
			break ;
	}
	return (NULL);
}
