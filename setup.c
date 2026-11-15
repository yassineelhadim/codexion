/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/11/15 18:08:30 by mberouak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	parse_number(const char *str, long long *out)
{
	long long	val;
	int			idx;

	if (!str || !str[0])
		return (-1);
	val = 0;
	idx = 0;
	while (str[idx])
	{
		if (str[idx] < '0' || str[idx] > '9')
			return (-1);
		val = val * 10 + (str[idx] - '0');
		if (val > 2000000000LL)
			return (-1);
		idx++;
	}
	*out = val;
	return (0);
}

static void	save_args(t_sim *sim, long long *vals)
{
	sim->coders_count = (int)vals[0];
	sim->burnout_ms = vals[1];
	sim->compile_ms = vals[2];
	sim->debug_ms = vals[3];
	sim->refactor_ms = vals[4];
	sim->goal_compiles = vals[5];
	sim->cooldown_ms = vals[6];
}

int	parse_args(int argc, char **argv, t_sim *sim)
{
	long long	vals[7];
	int			idx;

	if (argc != 9)
		return (-1);
	idx = 0;
	while (idx < 7)
	{
		if (parse_number(argv[idx + 1], &vals[idx]))
			return (-1);
		idx++;
	}
	if (vals[0] < 1)
		return (-1);
	if (!strcmp(argv[8], "fifo"))
		sim->mode = MODE_FIFO;
	else if (!strcmp(argv[8], "edf"))
		sim->mode = MODE_EDF;
	else
		return (-1);
	save_args(sim, vals);
	return (0);
}

static void	init_arrays(t_sim *sim)
{
	int	id;

	id = 0;
	while (id < sim->coders_count)
	{
		sim->dongles[id].in_use = 0;
		sim->dongles[id].ready_at_ms = 0;
		sim->last_compile_ms[id] = 0;
		sim->compiles_done[id] = 0;
		sim->is_waiting[id] = 0;
		id++;
	}
}

int	init_sim(t_sim *sim)
{
	sim->dongles = malloc(sizeof(t_dongle) * sim->coders_count);
	sim->last_compile_ms = malloc(sizeof(long long) * sim->coders_count);
	sim->compiles_done = malloc(sizeof(long long) * sim->coders_count);
	sim->is_waiting = malloc(sizeof(int) * sim->coders_count);
	sim->req_priority = malloc(sizeof(long long) * sim->coders_count);
	sim->req_order = malloc(sizeof(long long) * sim->coders_count);
	if (!sim->dongles || !sim->last_compile_ms || !sim->compiles_done
		|| !sim->is_waiting || !sim->req_priority || !sim->req_order
		|| queue_init(&sim->queue, sim->coders_count))
		return (-1);
	init_arrays(sim);
	sim->stopped = 0;
	sim->next_ticket = 0;
	if (sim->goal_compiles == 0)
		sim->stopped = 1;
	pthread_mutex_init(&sim->lock, NULL);
	pthread_mutex_init(&sim->print_lock, NULL);
	pthread_cond_init(&sim->wake_signal, NULL);
	return (0);
}
