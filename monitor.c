/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/09/07 22:47:09 by mberouak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long long	earliest_deadline(t_env *env, int *who)
{
	long long	deadline;
	long long	d;
	int			i;

	deadline = LLONG_MAX;
	*who = -1;
	i = 0;
	while (i < env->n)
	{
		d = env->last_start[i] + env->t_burnout;
		if (d < deadline)
		{
			deadline = d;
			*who = i;
		}
		i++;
	}
	return (deadline);
}

void	*monitor_routine(void *arg)
{
	t_env			*env;
	long long		deadline;
	int				who;

	env = arg;
	pthread_mutex_lock(&env->lock);
	while (!env->stop)
	{
		deadline = earliest_deadline(env, &who);
		if (now_ms(env) >= deadline)
		{
			env->stop = 1;
			log_state_locked(env, who, "burned out");
			pthread_cond_broadcast(&env->cond);
			break ;
		}
		pthread_mutex_unlock(&env->lock);
		usleep(1000);
		pthread_mutex_lock(&env->lock);
	}
	pthread_mutex_unlock(&env->lock);
	return (NULL);
}
