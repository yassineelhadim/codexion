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

long long	now_ms(t_env *env)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec - env->t0.tv_sec) * 1000LL
		+ (tv.tv_usec - env->t0.tv_usec) / 1000LL);
}

void	ts_add_ms(struct timespec *ts, t_env *env, long long ms)
{
	long long	us;

	us = (long long)env->t0.tv_sec * 1000000LL + env->t0.tv_usec + ms * 1000LL;
	ts->tv_sec = us / 1000000LL;
	ts->tv_nsec = (us % 1000000LL) * 1000LL;
}

void	log_state(t_env *env, int id, const char *msg)
{
	pthread_mutex_lock(&env->lock);
	if (!env->stop)
		log_state_locked(env, id, msg);
	pthread_mutex_unlock(&env->lock);
}

void	log_state_locked(t_env *env, int id, const char *msg)
{
	pthread_mutex_lock(&env->log_lock);
	printf("%lld %d %s\n", now_ms(env), id + 1, msg);
	pthread_mutex_unlock(&env->log_lock);
}

int	should_stop(t_env *env)
{
	int	s;

	pthread_mutex_lock(&env->lock);
	s = env->stop;
	pthread_mutex_unlock(&env->lock);
	return (s);
}
