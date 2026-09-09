#include "p_h.h"

static int	init_heap(t_table *table)
{
	int	n;

	n = table->config.nb_coders;
	table->heap.nodes = malloc(sizeof(t_heap_node) * n);
	if (!table->heap.nodes)
	{
		printf("something here");
		return (1);
	}
	table->heap.size = 0;
	table->heap.capacity = n;
	table->heap.next_seq = 0;
	return (0);
}

static int	init_coders(t_table *table)
{
	int	i;
	int	n;

	n = table->config.nb_coders;
	table->coders = malloc(sizeof(t_coder) * n);
	if (!table->coders)
	{
		printf("something here");
		return (1);
	}
	i = 0;
	while (i < n)
	{
		table->coders[i].coder_id = i + 1;
		table->coders[i].right_dongle = i;
		table->coders[i].left_dongle = (i + 1) % n;
		table->coders[i].last_compile_start = 0;
		table->coders[i].nb_compiles = 0;
		table->coders[i].state = STATE_WAITING;
		table->coders[i].table = table;
		i++;
	}
	return (0);
}

static int	init_table_fields(t_table *table, t_config *config)
{
	table->config = *config;
	table->stop = 0;
	table->coders = NULL;
	table->dongles = NULL;
	table->heap.nodes = NULL;
	table->start_time = get_time();
	if (pthread_mutex_init(&table->mutex, NULL) != 0)
		return (1);
	if (pthread_cond_init(&table->conduit, NULL) != 0)
	{
		pthread_mutex_destroy(&table->mutex);
		return (1);
	}
	return (0);
}

int	init_table(t_table *table, t_config *config)
{
	if (init_table_fields(table, config) != 0)
		return (1);
	if (init_dongles(table) || init_heap(table) || init_coders(table))
	{
		free(table->heap.nodes);
		free(table->coders);
		pthread_mutex_destroy(&table->mutex);
		pthread_cond_destroy(&table->conduit);
		return (1);
	}
	return (0);
}