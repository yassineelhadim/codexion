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

static int	all_done(t_env *env)
{
	int	i;

	i = 0;
	while (i < env->n)
	{
		if (env->done[i] < env->compiles_required)
			return (0);
		i++;
	}
	return (1);
}

static int	compile_once(t_env *env, int id)
{
	int	stop;

	sleep_ms(env, env->t_compile);
	release_dongles(env, id);
	pthread_mutex_lock(&env->lock);
	env->done[id]++;
	if (all_done(env))
	{
		env->stop = 1;
		pthread_cond_broadcast(&env->cond);
	}
	stop = env->stop;
	pthread_mutex_unlock(&env->lock);
	return (!stop);
}

static int	activity(t_env *env, int id, const char *msg, long long time)
{
	pthread_mutex_lock(&env->lock);
	if (env->stop)
	{
		pthread_mutex_unlock(&env->lock);
		return (0);
	}
	log_state_locked(env, id, msg);
	pthread_mutex_unlock(&env->lock);
	sleep_ms(env, time);
	return (!should_stop(env));
}

void	*coder_routine(void *arg)
{
	t_ctx	*c;
	t_env	*env;
	int		id;

	c = arg;
	env = c->env;
	id = c->id;
	free(c);
	while (1)
	{
		if (!acquire_dongles(env, id))
			break ;
		if (!compile_once(env, id))
			break ;
		if (!activity(env, id, "is debugging", env->t_debug))
			break ;
		if (!activity(env, id, "is refactoring", env->t_refactor))
			break ;
	}
	return (NULL);
}
