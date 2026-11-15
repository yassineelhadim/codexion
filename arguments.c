/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/09/07 22:47:09 by mberouak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	parse_unsigned_integer(const char *s, long long *out)
{
	long long	v;
	int			i;

	if (!s || !s[0])
		return (-1);
	v = 0;
	i = 0;
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (-1);
		v = v * 10 + (s[i] - '0');
		if (v > 2000000000LL)
			return (-1);
		i++;
	}
	*out = v;
	return (0);
}

static void	assign_simulation_settings(t_env *env, long long *v)
{
	env->coder_count = (int)v[0];
	env->burnout_interval_ms = v[1];
	env->compile_duration_ms = v[2];
	env->debug_duration_ms = v[3];
	env->refactor_duration_ms = v[4];
	env->required_compile_count = v[5];
	env->dongle_cooldown_ms = v[6];
}

int	parse_simulation_arguments(int argc, char **argv, t_env *env)
{
	long long	v[7];
	int			i;

	if (argc != 9)
		return (-1);
	i = 0;
	while (i < 7)
	{
		if (parse_unsigned_integer(argv[i + 1], &v[i]))
			return (-1);
		i++;
	}
	if (v[0] < 1)
		return (-1);
	if (!strcmp(argv[8], "fifo"))
		env->scheduler_policy = SCHED_FIFO_T;
	else if (!strcmp(argv[8], "edf"))
		env->scheduler_policy = SCHED_EDF_T;
	else
		return (-1);
	assign_simulation_settings(env, v);
	return (0);
}

static void	initialize_state_arrays(t_env *env)
{
	int	i;

	i = 0;
	while (i < env->coder_count)
	{
		env->dongle_states[i].is_taken = 0;
		env->dongle_states[i].available_after_ms = 0;
		env->last_compile_start_ms[i] = 0;
		env->completed_compiles[i] = 0;
		env->has_pending_request[i] = 0;
		i++;
	}
}

int	initialize_simulation_environment(t_env *env)
{
	env->dongle_states = malloc(sizeof(t_dongle) * env->coder_count);
	env->last_compile_start_ms = malloc(sizeof(long long) * env->coder_count);
	env->completed_compiles = malloc(sizeof(long long) * env->coder_count);
	env->has_pending_request = malloc(sizeof(int) * env->coder_count);
	env->priority_primary = malloc(sizeof(long long) * env->coder_count);
	env->priority_secondary = malloc(sizeof(long long) * env->coder_count);
	if (!env->dongle_states || !env->last_compile_start_ms
		|| !env->completed_compiles || !env->has_pending_request
		|| !env->priority_primary || !env->priority_secondary
		|| priority_heap_init(&env->request_heap, env->coder_count))
		return (-1);
	initialize_state_arrays(env);
	env->simulation_stopped = 0;
	env->next_request_sequence = 0;
	if (env->required_compile_count == 0)
		env->simulation_stopped = 1;
	pthread_mutex_init(&env->state_mutex, NULL);
	pthread_mutex_init(&env->output_mutex, NULL);
	pthread_cond_init(&env->state_changed, NULL);
	return (0);
}
