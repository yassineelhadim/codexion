#include "p_h.h"

static void	set_start_time(t_table *table)
{
	int	i;

	table->start_time = get_time();
	i = 0;
	while (i < table->config.nb_coders)
	{
		table->coders[i].last_compile_start = table->start_time;
		i++;
	}
}

static void	join_coders(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->config.nb_coders)
	{
		pthread_join(table->coders[i].thread, NULL);
		i++;
	}
}

static int	launch_coders(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->config.nb_coders)
	{
		if (pthread_create(&table->coders[i].thread, NULL,
				coder_routine, &table->coders[i]) != 0)
		{
			table->stop = 1;
			pthread_cond_broadcast(&table->conduit);
			while (--i >= 0)
				pthread_join(table->coders[i].thread, NULL);
			return (1);
		}
		i++;
	}
	return (0);
}

static void	join_all(t_table *table, pthread_t monitor)
{
	join_coders(table);
	pthread_join(monitor, NULL);
}

int	launch_simulation(t_table *table)
{
	pthread_t	monitor;

	set_start_time(table);
	if (launch_coders(table))
		return (1);
	if (pthread_create(&monitor, NULL, monitor_routine, table) != 0)
	{
		table->stop = 1;
		pthread_cond_broadcast(&table->conduit);
		join_coders(table);
		return (1);
	}
	join_all(table, monitor);
	return (0);
}