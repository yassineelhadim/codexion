/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   c_monitor.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 20:29:19 by yel-hadi          #+#    #+#             */
/*   Updated: 2026/09/10 20:30:46 by yel-hadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "p_h.h"
#include <unistd.h>

static int  check_burnout(t_table *table)
{
    int         i;
    long long   now;
    long long   elapsed;

    i = 0;
    now = get_time();
    while (i < table->config.nb_coders)
    {
        if (table->coders[i].state == STATE_BURNOUT)
        {
            i++;
            continue ;
        }
        //elapsed is when the coder started compiling
        elapsed = now - table->coders[i].last_compile_start;
        if (elapsed >= table->config.time_to_burnout)
        {
            log_locked(table, table->coders[i].coder_id, "burned out");
            table->stop = 1;
            pthread_cond_broadcast(&table->conduit);
            return (1);
        }
        i++;
    }
    return (0);
}

static int  check_all_done(t_table *table)
{
    int     i;
    int     done;

    i = 0;
    done = 0;
    // I need to calculate done
    // if done equals the nb of coders it means
    // that all the coders have finished their job.
    while (i < table->config.nb_coders)
    {
        if (table->coders[i].nb_compiles
            >= table->config.number_of_compiles_required)
            done++;
        i++;
    }
    if (done == table->config.nb_coders)
    {
        table->stop = 1;
        pthread_cond_broadcast(&table->conduit);
        return (1);
    }
    return (0);
}

void    *monitor_routine(void *arg)
{
    t_table *table;

    table = (t_table *)arg;
    while (1)
    {
        pthread_mutex_lock(&table->mutex);
        if (table->stop || check_burnout(table) || check_all_done(table))
        {
            pthread_mutex_unlock(&table->mutex);
            return (NULL);
        }
        pthread_mutex_unlock(&table->mutex);
        usleep(1000);
    }
}
