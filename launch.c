#include "p_h.h"

static void	set_start_time(t_table *table)
{
	int		i;
	long long	now;

	now = get_time();
	table->start_time = now;
	i = 0;
	while (i < table->config.nb_coders)
	{
		table->coders[i].last_compile_start = now;
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
			pthread_mutex_lock(&table->mutex);
			table->stop = 1;
			pthread_mutex_unlock(&table->mutex);
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

	if (launch_coders(table))
		return (1);
	set_start_time(table);
	if (pthread_create(&monitor, NULL, monitor_routine, table) != 0)
	{
		pthread_mutex_lock(&table->mutex);
		table->stop = 1;
		pthread_mutex_unlock(&table->mutex);
		pthread_cond_broadcast(&table->conduit);
		join_coders(table);
		return (1);
	}
	join_all(table, monitor);
	return (0);
}
