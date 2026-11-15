/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/11/15 18:08:30 by mberouak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	show_usage(char *prog)
{
	fprintf(stderr, "Usage: %s number_of_coders time_to_burnout "
		"time_to_compile time_to_debug time_to_refactor "
		"number_of_compiles_required dongle_cooldown scheduler(fifo|edf)\n"
		"All numeric arguments must be non-negative integers.\n", prog);
}

static int	start_sim(int argc, char **argv, t_sim *sim)
{
	if (parse_args(argc, argv, sim))
	{
		show_usage(argv[0]);
		return (1);
	}
	if (init_sim(sim))
	{
		fprintf(stderr, "Error: allocation failed\n");
		return (1);
	}
	gettimeofday(&sim->start_time, NULL);
	return (0);
}

static pthread_t	*spawn_threads(t_sim *sim, pthread_t *monitor)
{
	pthread_t	*coders;
	t_coder_arg	*arg;
	int			id;

	coders = malloc(sizeof(pthread_t) * sim->coders_count);
	if (!coders)
		return (NULL);
	pthread_create(monitor, NULL, monitor_loop, sim);
	id = 0;
	while (id < sim->coders_count)
	{
		arg = malloc(sizeof(t_coder_arg));
		arg->sim = sim;
		arg->coder_id = id;
		pthread_create(&coders[id], NULL, coder_loop, arg);
		id++;
	}
	return (coders);
}

static void	join_threads(t_sim *sim, pthread_t *coders, pthread_t monitor)
{
	int	id;

	id = 0;
	while (id < sim->coders_count)
		pthread_join(coders[id++], NULL);
	pthread_join(monitor, NULL);
}

int	main(int argc, char **argv)
{
	t_sim		sim;
	pthread_t	monitor;
	pthread_t	*coders;

	if (start_sim(argc, argv, &sim))
		return (1);
	coders = spawn_threads(&sim, &monitor);
	if (!coders)
	{
		free_sim(&sim);
		return (1);
	}
	join_threads(&sim, coders, monitor);
	free(coders);
	free_sim(&sim);
	return (0);
}
