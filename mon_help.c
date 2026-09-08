#include "p_h.h"

long long   get_time(void)
{
    struct timeval  tv;

    //I need to understand the struct, and what is tv.tv_sec
    //I need to learn how to use structs in the code.
    gettimeofday(&tv, NULL);
    return((long long)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void        log_locked(t_table *table, int coder_id, char *action)
{
    long long   timestmp;

    if (table->stop)
        return;
    timestmp = get_time() - table->start_time;
    printf("%lld %d %s\n", timestmp, coder_id, action);
}

void        log_action(t_table *table, int coder_id, char *action)
{
    pthread_mutex_lock(&table->mutex);
    log_locked(table, coder_id, action);
    pthread_mutex_unlock(&table->mutex);
}