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
	while (i < env->coder_count)
	{
		d = env->last_compile_start_ms[i] + env->burnout_interval_ms;
		if (d < deadline)
		{
			deadline = d;
			*who = i;
		}
		i++;
	}
	return (deadline);
}

void	*run_burnout_monitor(void *arg)
{
	t_env			*env;
	long long		deadline;
	int				who;

	env = arg;
	pthread_mutex_lock(&env->state_mutex);
	while (!env->simulation_stopped)
	{
		deadline = earliest_deadline(env, &who);
		if (elapsed_ms(env) >= deadline)
		{
			env->simulation_stopped = 1;
			write_state_locked(env, who, "burned out");
			pthread_cond_broadcast(&env->state_changed);
			break ;
		}
		pthread_mutex_unlock(&env->state_mutex);
		usleep(1000);
		pthread_mutex_lock(&env->state_mutex);
	}
	pthread_mutex_unlock(&env->state_mutex);
	return (NULL);
}
