/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 20:29:19 by yel-hadi          #+#    #+#             */
/*   Updated: 2026/09/10 20:29:26 by yel-hadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "p_h.h"

static void	cleanup_dongles(t_table *table, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		free(table->dongles[i].waiting_heap.nodes);
		pthread_mutex_destroy(&table->dongles[i].mutex);
		pthread_cond_destroy(&table->dongles[i].cond);
		i++;
	}
	free(table->dongles);
}

static int	init_dongle_heap(t_table *table, int i)
{
	int	n;

	n = table->config.nb_coders;
	table->dongles[i].waiting_heap.nodes = malloc(sizeof(t_heap_node) * n);
	if (!table->dongles[i].waiting_heap.nodes)
		return (1);
	table->dongles[i].waiting_heap.size = 0;
	table->dongles[i].waiting_heap.capacity = n;
	table->dongles[i].waiting_heap.next_seq = 0;
	return (0);
}

static int	init_one_dongle(t_table *table, int i)
{
	table->dongles[i].dongle_id = i;
	table->dongles[i].availability = 1;
	table->dongles[i].helder = -1;
	table->dongles[i].time_of_release = 0;
	table->dongles[i].ever_released = 0;
	if (init_dongle_heap(table, i))
		return (1);
	if (pthread_mutex_init(&table->dongles[i].mutex, NULL) != 0)
		return (1);
	if (pthread_cond_init(&table->dongles[i].cond, NULL) != 0)
		return (1);
	return (0);
}

int	init_dongles(t_table *table)
{
	int	i;
	int	n;

	n = table->config.nb_coders;
	table->dongles = malloc(sizeof(t_dongle) * n);
	if (!table->dongles)
		return (1);
	i = 0;
	while (i < n)
	{
		if (init_one_dongle(table, i))
		{
			cleanup_dongles(table, i);
			return (1);
		}
		i++;
	}
	return (0);
}