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

static int	parse_uint(const char *s, long long *out)
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

static void	set_values(t_env *env, long long *v)
{
	env->n = (int)v[0];
	env->t_burnout = v[1];
	env->t_compile = v[2];
	env->t_debug = v[3];
	env->t_refactor = v[4];
	env->compiles_required = v[5];
	env->cooldown = v[6];
}

int	parse_args(int argc, char **argv, t_env *env)
{
	long long	v[7];
	int			i;

	if (argc != 9)
		return (-1);
	i = 0;
	while (i < 7)
	{
		if (parse_uint(argv[i + 1], &v[i]))
			return (-1);
		i++;
	}
	if (v[0] < 1)
		return (-1);
	if (!strcmp(argv[8], "fifo"))
		env->scheduler = SCHED_FIFO_T;
	else if (!strcmp(argv[8], "edf"))
		env->scheduler = SCHED_EDF_T;
	else
		return (-1);
	set_values(env, v);
	return (0);
}

static void	init_arrays(t_env *env)
{
	int	i;

	i = 0;
	while (i < env->n)
	{
		env->dongles[i].taken = 0;
		env->dongles[i].free_at = 0;
		env->last_start[i] = 0;
		env->done[i] = 0;
		env->waiting[i] = 0;
		i++;
	}
}

int	env_init(t_env *env)
{
	env->dongles = malloc(sizeof(t_dongle) * env->n);
	env->last_start = malloc(sizeof(long long) * env->n);
	env->done = malloc(sizeof(long long) * env->n);
	env->waiting = malloc(sizeof(int) * env->n);
	env->k1 = malloc(sizeof(long long) * env->n);
	env->k2 = malloc(sizeof(long long) * env->n);
	if (!env->dongles || !env->last_start || !env->done
		|| !env->waiting || !env->k1 || !env->k2
		|| heap_init(&env->work, env->n))
		return (-1);
	init_arrays(env);
	env->stop = 0;
	env->seq = 0;
	if (env->compiles_required == 0)
		env->stop = 1;
	pthread_mutex_init(&env->lock, NULL);
	pthread_mutex_init(&env->log_lock, NULL);
	pthread_cond_init(&env->cond, NULL);
	return (0);
}
