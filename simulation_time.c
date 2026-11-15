/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/09/07 22:47:09 by mberouak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	elapsed_ms(t_env *env)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec - env->simulation_start_time.tv_sec) * 1000LL
		+ (tv.tv_usec - env->simulation_start_time.tv_usec) / 1000LL);
}

void	set_deadline_timespec(struct timespec *ts, t_env *env, long long ms)
{
	long long	us;

	us = (long long)env->simulation_start_time.tv_sec * 1000000LL
		+ env->simulation_start_time.tv_usec + ms * 1000LL;
	ts->tv_sec = us / 1000000LL;
	ts->tv_nsec = (us % 1000000LL) * 1000LL;
}

void	write_state(t_env *env, int id, const char *msg)
{
	pthread_mutex_lock(&env->state_mutex);
	if (!env->simulation_stopped)
		write_state_locked(env, id, msg);
	pthread_mutex_unlock(&env->state_mutex);
}

void	write_state_locked(t_env *env, int id, const char *msg)
{
	pthread_mutex_lock(&env->output_mutex);
	printf("%lld %d %s\n", elapsed_ms(env), id + 1, msg);
	pthread_mutex_unlock(&env->output_mutex);
}

int	simulation_stopped(t_env *env)
{
	int	s;

	pthread_mutex_lock(&env->state_mutex);
	s = env->simulation_stopped;
	pthread_mutex_unlock(&env->state_mutex);
	return (s);
}
