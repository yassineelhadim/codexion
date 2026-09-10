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

static void	set_dongle_order(t_coder *coder, int *first, int *second)
{
	if (coder->left_dongle < coder->right_dongle)
	{
		*first = coder->left_dongle;
		*second = coder->right_dongle;
	}
	else
	{
		*first = coder->right_dongle;
		*second = coder->left_dongle;
	}
}

static void	do_compile(t_table *table, t_coder *coder, int first, int second)
{
	take_dongle(table, first, coder->coder_id);
	take_dongle(table, second, coder->coder_id);
	pthread_mutex_lock(&table->mutex);
	coder->last_compile_start = get_time();
	coder->state = STATE_COMPILING;
	log_locked(table, coder->coder_id, "is compiling");
	pthread_mutex_unlock(&table->mutex);
	usleep(table->config.time_to_compile * 1000);
	release_dongle(table, first);
	release_dongle(table, second);
}

static void	do_debug_and_refactor(t_table *table, t_coder *coder)
{
	pthread_mutex_lock(&table->mutex);
	if (table->stop)
	{
		pthread_mutex_unlock(&table->mutex);
		return ;
	}
	coder->state = STATE_DEBUGGING;
	log_locked(table, coder->coder_id, "is debugging");
	pthread_mutex_unlock(&table->mutex);
	usleep(table->config.time_to_debug * 1000);
	pthread_mutex_lock(&table->mutex);
	if (table->stop)
	{
		pthread_mutex_unlock(&table->mutex);
		return ;
	}
	coder->state = STATE_REFACTORING;
	log_locked(table, coder->coder_id, "is refactoring");
	pthread_mutex_unlock(&table->mutex);
	usleep(table->config.time_to_refactor * 1000);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_table	*table;
	int		first;
	int		second;

	coder = (t_coder *)arg;
	table = coder->table;
	set_dongle_order(coder, &first, &second);
	while (1)
	{
		pthread_mutex_lock(&table->mutex);
		if (table->stop)
		{
			pthread_mutex_unlock(&table->mutex);
			break ;
		}
		pthread_mutex_unlock(&table->mutex);
		do_compile(table, coder, first, second);
		pthread_mutex_lock(&table->mutex);
		coder->nb_compiles++;
		pthread_mutex_unlock(&table->mutex);
		do_debug_and_refactor(table, coder);
	}
	return (NULL);
}
