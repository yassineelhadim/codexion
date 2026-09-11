/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 00:00:00 by yel-hadi       #+#    #+#             */
/*   Updated: 2026/09/11 00:00:00 by yel-hadi      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

/*
** One condition variable + one mutex protect the whole simulation state
** (dongles, heaps, coder data, stop flag). Because every shared field is
** touched under the same lock, no two threads can ever grab two locks in
** a different order: circular-wait deadlocks are impossible by design.
*/

typedef enum e_scheduler
{
	SCHEDULER_FIFO = 0,
	SCHEDULER_EDF = 1
}	t_scheduler;

typedef enum e_state
{
	STATE_THINKING = 0,
	STATE_WAITING_DONGLES,
	STATE_COMPILING,
	STATE_DEBUGGING,
	STATE_REFACTORING,
	STATE_BURNED_OUT
}	t_state;

typedef long long	t_millis;

typedef struct s_config
{
	int			nb_coders;
	long		time_to_burnout;
	long		time_to_compile;
	long		time_to_debug;
	long		time_to_refactor;
	int			nb_compiles_required;
	long		dongle_cooldown;
	t_scheduler	scheduler;
}	t_config;

/*
** Min-heap entry kept in each dongle's waiting queue.
**   deadline    = last_compile_start + time_to_burnout (EDF priority)
**   arrival_seq = global counter incremented per request (FIFO priority,
**                 also the deterministic tie-breaker for EDF)
*/
typedef struct s_request
{
	int			coder_id;
	long long	deadline;
	long long	arrival_seq;
}	t_request;

typedef struct s_heap
{
	t_request	*items;
	int			size;
	int			capacity;
}	t_heap;

/*
** One dongle per coder, placed between neighbour coders.
** All fields below are protected by sim->lock.
*/
typedef struct s_dongle
{
	int			id;
	int			holder;			/* coder_id holding it, -1 if on the table */
	int			hands;			/* hands currently plugged (2 max, n == 1) */
	long long	cooldown_until;	/* sim time before which it cannot be taken */
	t_heap		waiting;		/* min-heap of pending requests */
}	t_dongle;

typedef struct s_sim		t_sim;

typedef struct s_coder
{
	int			id;
	pthread_t	thread;
	t_state		state;
	int			left;
	int			right;
	long long	deadline;		/* absolute burnout deadline (ms since epoch) */
	int			nb_compiles;
	t_sim		*sim;
}	t_coder;

typedef struct s_sim
{
	t_config		config;
	t_dongle		*dongles;
	t_coder			*coders;
	pthread_mutex_t	lock;
	pthread_cond_t	event;
	int				lock_ready;
	int				event_ready;
	long long		start_time;		/* epoch ms, set once before threads run */
	long long		next_arrival;	/* monotonically increasing request id */
	int				stop;			/* 1 = simulation finished */
	int				burnout_id;		/* coder that died, 0 = nobody */
}	t_sim;

/* parsing.c */
int		parse_args(int argc, char **argv, t_config *config);

/* parsing_utils.c */
int		parse_positive_long(const char *str, long *out);
int		parse_non_negative_long(const char *str, long *out);
int		parse_scheduler(const char *str, t_scheduler *out);
void	print_usage(const char *prog);

/* scheduler.c (binary min-heap, no standard priority queue) */
int		request_wins(t_scheduler sched, const t_request *a, const t_request *b);
void	swap_requests(t_request *a, t_request *b);
void	heap_push(t_heap *heap, t_scheduler sched, const t_request *request);
void	heap_pop(t_heap *heap, t_scheduler sched);
void	heap_remove_coder(t_heap *heap, t_scheduler sched, int coder_id);
int		heap_top_is(const t_heap *heap, int coder_id);

/* sim_setup.c */
int		sim_init(t_sim *sim, const t_config *config);
void	sim_destroy(t_sim *sim);

/* sim_launch.c */
int		sim_run(t_sim *sim);

/* dongle_ops.c */
void	acquire_dongles(t_sim *sim, t_coder *coder);
void	release_dongles(t_sim *sim, t_coder *coder);
void	withdraw_requests(t_sim *sim, t_coder *coder);
void	grant_ready_pairs(t_sim *sim);
int		coder_has_both_dongles(t_sim *sim, t_coder *coder);

/* monitor.c */
void	*monitor_routine(void *arg);

/* coder.c */
void	*coder_routine(void *arg);

/* coder_utils.c */
void	sleep_interruptible(t_sim *sim, long duration_ms);
t_millis		time_in_ms(void);
void	log_state(t_sim *sim, int coder_id, const char *action);

#endif
