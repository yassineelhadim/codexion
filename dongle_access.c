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

static int	grant_requested_pair(t_env *env, long long now)
{
	t_hnode	top;
	int		a;
	int		b;

	top = priority_heap_extract_min(&env->request_heap);
	a = top.coder_index;
	b = (top.coder_index - 1 + env->coder_count) % env->coder_count;
	if (a == b || env->dongle_states[a].is_taken
		|| env->dongle_states[b].is_taken
		|| now < env->dongle_states[a].available_after_ms
		|| now < env->dongle_states[b].available_after_ms)
		return (0);
	env->dongle_states[a].is_taken = 1;
	env->dongle_states[b].is_taken = 1;
	env->has_pending_request[top.coder_index] = 0;
	env->last_compile_start_ms[top.coder_index] = now;
	write_state_locked(env, top.coder_index, "has taken a dongle");
	write_state_locked(env, top.coder_index, "has taken a dongle");
	write_state_locked(env, top.coder_index, "is compiling");
	return (1);
}

static void	arbitrate_dongles(t_env *env)
{
	long long	now;
	int			id;
	int			granted;

	if (env->simulation_stopped)
		return ;
	now = elapsed_ms(env);
	env->request_heap.count = 0;
	id = 0;
	while (id < env->coder_count)
	{
		if (env->has_pending_request[id])
			priority_heap_push(&env->request_heap,
				env->priority_primary[id], env->priority_secondary[id], id);
		id++;
	}
	granted = 0;
	while (!priority_heap_is_empty(&env->request_heap))
	{
		if (grant_requested_pair(env, now))
			granted = 1;
	}
	if (granted)
		pthread_cond_broadcast(&env->state_changed);
}

static int	wait_for_dongle_pair(t_env *env, int id)
{
	if (env->simulation_stopped)
	{
		env->has_pending_request[id] = 0;
		return (0);
	}
	if (!env->has_pending_request[id])
		return (1);
	pthread_mutex_unlock(&env->state_mutex);
	usleep(1000);
	pthread_mutex_lock(&env->state_mutex);
	arbitrate_dongles(env);
	return (-1);
}

int	request_dongle_pair(t_env *env, int id)
{
	long long		myseq;
	int				result;

	pthread_mutex_lock(&env->state_mutex);
	myseq = (env->next_request_sequence)++;
	if (env->scheduler_policy == SCHED_FIFO_T)
		env->priority_primary[id] = myseq;
	else
		env->priority_primary[id] = env->last_compile_start_ms[id]
			+ env->burnout_interval_ms;
	env->priority_secondary[id] = myseq;
	env->has_pending_request[id] = 1;
	arbitrate_dongles(env);
	while (1)
	{
		result = wait_for_dongle_pair(env, id);
		if (result != -1)
		{
			pthread_mutex_unlock(&env->state_mutex);
			return (result);
		}
	}
}

void	release_dongle_pair(t_env *env, int id)
{
	int			a;
	int			b;
	long long	now;

	a = id;
	b = (id - 1 + env->coder_count) % env->coder_count;
	pthread_mutex_lock(&env->state_mutex);
	now = elapsed_ms(env);
	env->dongle_states[a].is_taken = 0;
	env->dongle_states[a].available_after_ms = now + env->dongle_cooldown_ms;
	if (b != a)
	{
		env->dongle_states[b].is_taken = 0;
		env->dongle_states[b].available_after_ms
			= now + env->dongle_cooldown_ms;
	}
	arbitrate_dongles(env);
	pthread_cond_broadcast(&env->state_changed);
	pthread_mutex_unlock(&env->state_mutex);
}
