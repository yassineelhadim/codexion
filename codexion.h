/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:33:32 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:33:33 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <string.h>
# include <stdbool.h>
# include <limits.h>

/*
** ENUMS
*/

typedef enum e_status
{
	SUCCESS = 0,
	FAILURE = 1
}	t_status;

typedef enum e_opcode
{
	LOCK,
	UNLOCK,
	INIT,
	DESTROY,
	SIGNAL,
	BROADCAST
}	t_opcode;

typedef enum e_scheduler
{
	FIFO_,
	EDF_
}	t_scheduler;

/*
** FORWARD DECLARATIONS
*/

typedef struct s_config		t_config;
typedef struct s_coder		t_coder;
typedef struct s_dongle		t_dongle;
typedef struct s_monitor	t_monitor;
typedef struct s_request	t_request;

/*
** TYPE DEFINITIONS
*/

typedef int					(*t_comparator)(t_request *a, t_request *b);

/*
** STRUCTURES
*/

struct s_request
{
	int			coder_id;
	long long	timestamp;
	long long	deadline;
};

struct s_dongle
{
	int				id;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	long long		available_at;
	int				held_by;
	t_request		*queue;
	int				queue_size;
	int				queue_capacity;
	t_comparator	cmp;
};

struct s_coder
{
	int				id;
	pthread_t		thread;
	pthread_mutex_t	mutex;
	long long		last_compile_start;
	int				compile_count;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	t_dongle		*first_dongle;
	t_dongle		*second_dongle;
	t_config		*config;
};

struct s_monitor
{
	pthread_t		thread;
	pthread_mutex_t	mutex;
	bool			running;
};

struct s_config
{
	int				coder_count;
	long long		time_to_burnout;
	long long		time_to_compile;
	long long		time_to_debug;
	long long		time_to_refactor;
	int				compiles_required;
	long long		dongle_cooldown;
	t_scheduler		scheduler;
	t_coder			*coders;
	t_dongle		*dongles;
	t_monitor		*monitor;
	pthread_mutex_t	event_mutex;
	pthread_mutex_t	start_mutex;
	pthread_cond_t	start_cond;
	bool			sim_started;
	long long		started_at;
	bool			event_mutex_init;
	bool			start_mutex_init;
	bool			start_cond_init;
	bool			coders_allocated;
	bool			dongles_allocated;
	bool			monitor_allocated;
	int				coders_init_count;
	int				dongles_init_count;
};

/*
** PARSER (parser.c)
*/

t_status	parse_args(int argc, char **argv, t_config *config);

/*
** INIT (init.c)
*/

t_status	init_config(t_config *config);

/*
** SIMULATION (simulation.c)
*/

t_status	simulate(t_config *config);

/*
** CLEANUP (cleanup.c)
*/

void		cleanup(t_config *config);

/*
** UTILS (utils.c)
*/

long long	get_time_us(void);
long long	get_time_ms(void);
void		nap(t_config *config, long long us);

/*
** SAFE WRAPPERS (safe_wrappers.c)
*/

t_status	safe_mutex_handle(pthread_mutex_t *mutex, t_opcode opcode);
t_status	safe_cond_handle(pthread_cond_t *cond, t_opcode opcode);
t_status	safe_thread_create(pthread_t *thread,
				void *(*func)(void *), void *arg);
t_status	safe_thread_join(pthread_t thread);

/*
** CONSOLE (console.c)
*/

t_status	event(t_config *config, t_coder *coder, const char *msg);
t_status	event_burnout(t_config *config, t_coder *coder);

/*
** GETTERS (getters.c)
*/

bool		get_sim_state(t_config *config);
int			get_compile_count(t_coder *coder);
long long	get_last_compile_start(t_coder *coder);

/*
** SETTERS (setters.c)
*/

void		set_sim_state(t_config *config, bool state);
void		increment_compile_count(t_coder *coder);
void		set_last_compile_start(t_coder *coder, long long time);

/*
** CODER (coder.c)
*/

void		*coder_routine(void *arg);

/*
** MONITOR (monitor.c)
*/

void		*monitor_routine(void *arg);

/*
** DONGLE (dongle.c)
*/

t_status	acquire_two_dongles(t_coder *coder);
t_status	acquire_dongle(t_coder *coder, t_dongle *d);
void		release_dongle(t_coder *coder, t_dongle *d);
void		release_two_dongles(t_coder *coder);

/*
** HEAP (heap.c)
*/

void		heap_push(t_dongle *dongle, t_request req);
t_request	heap_pop(t_dongle *dongle);
t_request	heap_peek(t_dongle *dongle);
void		heap_remove(t_dongle *dongle, int coder_id);

/*
** COMPARATORS (comparators.c)
*/

int			fifo_cmp(t_request *a, t_request *b);
int			edf_cmp(t_request *a, t_request *b);

#endif