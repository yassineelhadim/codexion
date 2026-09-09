#include "p_h.h"
#include <stdbool.h>

static bool dongle_available(long long cooldown, int helder, t_dongle *dg)
{
    if (get_time() - dg->time_of_release >= cooldown && helder == -1)
    {
        dg->availability = 1;
        return (true);
    }
    else
        return (false);
}

// How does pthread_mutex work, and why do we lock the table and unlock it
void    take_dongle(t_table *table, int dg_id, int coder_id)
{
    long long   cooldown;

    cooldown = table->config.dongle_cooldown;
    pthread_mutex_lock(&table->mutex);
    while (table->dongles[dg_id].availability == 0 && !table->stop)
        // what happens here?
        pthread_cond_wait(&table->conduit, &table->mutex);
    if (table->stop)
    {
        // what does unlock do?
        pthread_mutex_unlock(&table->mutex);
        return ;
    }
    if (dongle_available(cooldown, table->dongles[dg_id].helder , &table->dongles[dg_id]))
    {
        table->dongles[dg_id].availability = 0;
        table->dongles[dg_id].helder = coder_id;
    }
    else
    {
        pthread_mutex_unlock(&table->mutex);
        return ;
    }
    pthread_mutex_unlock(&table->mutex);
    log_locked(table, coder_id, "has taken a dongle");
}

void    release_dongle(t_table *table, int dongle_id)
{
    pthread_mutex_lock(&table->mutex);
    table->dongles[dongle_id].availability = 0;
    table->dongles[dongle_id].helder = -1;
    table->dongles[dongle_id].time_of_release = get_time();
    // what does the cond_broadcast do?
    pthread_cond_broadcast(&table->conduit);
    pthread_mutex_unlock(&table->mutex);
}