/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*                                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <yel-hadi.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 00:00:00 by yel-hadi          #+#    #+#             */
/*   Updated: 2026/11/12 00:00:00 by yel-hadi         ###   ########.fr       */
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

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	int				mutex_ready;
	int				id;
	int				holder;
	int				hands;
	long long		cooldown_until;
	t_heap			waiting;
}	t_dongle;

typedef struct s_sim	t_sim;

typedef struct s_coder
{
	int			id;
	pthread_t	thread;
	t_state		state;
	int			left;
	int			right;
	long long	deadline;		
	int			nb_compiles;
	t_sim		*sim;
}	t_coder;

struct s_sim
{
	t_config		config;
	t_dongle		*dongles;
	t_coder			*coders;
	pthread_mutex_t	lock;
	pthread_cond_t	event;
	int				lock_ready;
	int				event_ready;
	long long		start_time;		
	long long		next_arrival;
	int				stop;			
};

int			parse_args(int argc, char **argv, t_config *config);

int			parse_positive_long(const char *str, long *out);
int			parse_non_negative_long(const char *str, long *out);
int			parse_scheduler(const char *str, t_scheduler *out);
void		print_usage(const char *prog);

int			request_wins(t_scheduler sched, const t_request *a,
				const t_request *b);
void		swap_requests(t_request *a, t_request *b);
void		heap_push(t_heap *h, t_scheduler s, const t_request *r);
void		heap_pop(t_heap *heap, t_scheduler sched);
void		heap_remove_coder(t_heap *h, t_scheduler s, int id);
int			heap_top_is(const t_heap *heap, int coder_id);

int			sim_init(t_sim *sim, const t_config *config);
void		sim_destroy(t_sim *sim);

int			sim_run(t_sim *sim);

void		acquire_dongles(t_sim *sim, t_coder *coder);
void		release_dongles(t_sim *sim, t_coder *coder);
void		withdraw_requests(t_sim *sim, t_coder *coder);
void		lock_dongle_pair(t_dongle *left, t_dongle *right);
void		unlock_dongle_pair(t_dongle *left, t_dongle *right);
void		grant_ready_pairs(t_sim *sim);
int			coder_has_both_dongles(t_sim *sim, t_coder *coder);

void		*monitor_routine(void *arg);

void		*coder_routine(void *arg);

void		sleep_interruptible(t_sim *sim, long duration_ms);
long long	time_in_ms(void);
void		log_state(t_sim *sim, int coder_id, const char *action);

#endif
