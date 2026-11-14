/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/09/07 22:47:09 by mberouak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	print_usage(char *name)
{
	fprintf(stderr, "Usage: %s number_of_coders time_to_burnout "
		"time_to_compile time_to_debug time_to_refactor "
		"number_of_compiles_required dongle_cooldown scheduler(fifo|edf)\n"
		"All numeric arguments must be non-negative integers.\n", name);
}

static int	setup(int argc, char **argv, t_env *env)
{
	if (parse_args(argc, argv, env))
	{
		print_usage(argv[0]);
		return (1);
	}
	if (env_init(env))
	{
		fprintf(stderr, "Error: allocation failed\n");
		return (1);
	}
	gettimeofday(&env->t0, NULL);
	return (0);
}

static pthread_t	*start_coders(t_env *env, pthread_t *monitor)
{
	pthread_t	*coders;
	t_ctx		*c;
	int			i;

	coders = malloc(sizeof(pthread_t) * env->n);
	if (!coders)
		return (NULL);
	pthread_create(monitor, NULL, monitor_routine, env);
	i = 0;
	while (i < env->n)
	{
		c = malloc(sizeof(t_ctx));
		c->env = env;
		c->id = i;
		pthread_create(&coders[i], NULL, coder_routine, c);
		i++;
	}
	return (coders);
}

static void	wait_coders(t_env *env, pthread_t *coders, pthread_t monitor)
{
	int	i;

	i = 0;
	while (i < env->n)
		pthread_join(coders[i++], NULL);
	pthread_join(monitor, NULL);
}

int	main(int argc, char **argv)
{
	t_env		env;
	pthread_t	monitor;
	pthread_t	*coders;

	if (setup(argc, argv, &env))
		return (1);
	coders = start_coders(&env, &monitor);
	if (!coders)
	{
		env_destroy(&env);
		return (1);
	}
	wait_coders(&env, coders, monitor);
	free(coders);
	env_destroy(&env);
	return (0);
}
