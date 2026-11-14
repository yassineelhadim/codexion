/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/09/07 22:47:09 by mberouak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <limits.h>

# define SCHED_FIFO_T 0
# define SCHED_EDF_T 1

typedef struct s_hnode
{
	long long	k1;
	long long	k2;
	int			id;
}	t_hnode;

typedef struct s_heap
{
	t_hnode	*a;
	int		n;
	int		cap;
}	t_heap;

typedef struct s_dongle
{
	int			taken;
	long long	free_at;
}	t_dongle;

typedef struct s_env
{
	int				n;
	long long		t_burnout;
	long long		t_compile;
	long long		t_debug;
	long long		t_refactor;
	long long		compiles_required;
	long long		cooldown;
	int				scheduler;
	int				stop;
	long long		seq;
	struct timeval	t0;
	pthread_mutex_t	lock;
	pthread_mutex_t	log_lock;
	pthread_cond_t	cond;
	t_dongle		*dongles;
	long long		*last_start;
	long long		*done;
	int				*waiting;
	long long		*k1;
	long long		*k2;
	t_heap			work;
}	t_env;

typedef struct s_ctx
{
	t_env	*env;
	int		id;
}	t_ctx;

long long	now_ms(t_env *env);
void		ts_add_ms(struct timespec *ts, t_env *env, long long ms);
void		log_state(t_env *env, int id, const char *msg);
void		log_state_locked(t_env *env, int id, const char *msg);
int			should_stop(t_env *env);
void		sleep_ms(t_env *env, long long ms);

int			heap_init(t_heap *h, int cap);
void		heap_free(t_heap *h);
void		heap_push(t_heap *h, long long k1, long long k2, int id);
t_hnode		heap_extract_min(t_heap *h);
int			heap_empty(t_heap *h);
int			heap_less(t_hnode *x, t_hnode *y);
void		heap_swap(t_hnode *x, t_hnode *y);

int			acquire_dongles(t_env *env, int id);
void		release_dongles(t_env *env, int id);

void		*coder_routine(void *arg);

void		*monitor_routine(void *arg);

int			parse_args(int argc, char **argv, t_env *env);
int			env_init(t_env *env);
void		env_destroy(t_env *env);

#endif
