/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/09/07 22:47:09 by mberouak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	grant_dongle(t_env *env, long long now)
{
	t_hnode	top;
	int		a;
	int		b;

	top = heap_extract_min(&env->work);
	a = top.id;
	b = (top.id - 1 + env->n) % env->n;
	if (a == b || env->dongles[a].taken || env->dongles[b].taken
		|| now < env->dongles[a].free_at || now < env->dongles[b].free_at)
		return (0);
	env->dongles[a].taken = 1;
	env->dongles[b].taken = 1;
	env->waiting[top.id] = 0;
	env->last_start[top.id] = now;
	log_state_locked(env, top.id, "has taken a dongle");
	log_state_locked(env, top.id, "has taken a dongle");
	log_state_locked(env, top.id, "is compiling");
	return (1);
}

static void	arbitrate(t_env *env)
{
	long long	now;
	int			id;
	int			granted;

	if (env->stop)
		return ;
	now = now_ms(env);
	env->work.n = 0;
	id = 0;
	while (id < env->n)
	{
		if (env->waiting[id])
			heap_push(&env->work, env->k1[id], env->k2[id], id);
		id++;
	}
	granted = 0;
	while (!heap_empty(&env->work))
	{
		if (grant_dongle(env, now))
			granted = 1;
	}
	if (granted)
		pthread_cond_broadcast(&env->cond);
}

static int	wait_dongles(t_env *env, int id)
{
	if (env->stop)
	{
		env->waiting[id] = 0;
		return (0);
	}
	if (!env->waiting[id])
		return (1);
	pthread_mutex_unlock(&env->lock);
	usleep(1000);
	pthread_mutex_lock(&env->lock);
	arbitrate(env);
	return (-1);
}

int	acquire_dongles(t_env *env, int id)
{
	long long		myseq;
	int				result;

	pthread_mutex_lock(&env->lock);
	myseq = (env->seq)++;
	if (env->scheduler == SCHED_FIFO_T)
		env->k1[id] = myseq;
	else
		env->k1[id] = env->last_start[id] + env->t_burnout;
	env->k2[id] = myseq;
	env->waiting[id] = 1;
	arbitrate(env);
	while (1)
	{
		result = wait_dongles(env, id);
		if (result != -1)
		{
			pthread_mutex_unlock(&env->lock);
			return (result);
		}
	}
}

void	release_dongles(t_env *env, int id)
{
	int			a;
	int			b;
	long long	now;

	a = id;
	b = (id - 1 + env->n) % env->n;
	pthread_mutex_lock(&env->lock);
	now = now_ms(env);
	env->dongles[a].taken = 0;
	env->dongles[a].free_at = now + env->cooldown;
	if (b != a)
	{
		env->dongles[b].taken = 0;
		env->dongles[b].free_at = now + env->cooldown;
	}
	arbitrate(env);
	pthread_cond_broadcast(&env->cond);
	pthread_mutex_unlock(&env->lock);
}
