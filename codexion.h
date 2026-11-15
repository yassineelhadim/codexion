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
	long long	primary_priority;
	long long	secondary_priority;
	int			coder_index;
}	t_hnode;

typedef struct s_heap
{
	t_hnode	*nodes;
	int		count;
	int		capacity;
}	t_heap;

typedef struct s_dongle
{
	int				is_taken;
	long long		available_after_ms;
}	t_dongle;

typedef struct s_env
{
	int				coder_count;
	long long		burnout_interval_ms;
	long long		compile_duration_ms;
	long long		debug_duration_ms;
	long long		refactor_duration_ms;
	long long		required_compile_count;
	long long		dongle_cooldown_ms;
	int				scheduler_policy;
	int				simulation_stopped;
	long long		next_request_sequence;
	struct timeval	simulation_start_time;
	pthread_mutex_t	state_mutex;
	pthread_mutex_t	output_mutex;
	pthread_cond_t	state_changed;
	t_dongle		*dongle_states;
	long long		*last_compile_start_ms;
	long long		*completed_compiles;
	int				*has_pending_request;
	long long		*priority_primary;
	long long		*priority_secondary;
	t_heap			request_heap;
}	t_env;

typedef struct s_ctx
{
	t_env		*env;
	int			coder_index;
}	t_ctx;

long long	elapsed_ms(t_env *env);
void		set_deadline_timespec(struct timespec *ts, t_env *env,
				long long ms);
void		write_state(t_env *env, int id, const char *msg);
void		write_state_locked(t_env *env, int id, const char *msg);
int			simulation_stopped(t_env *env);
void		sleep_milliseconds(t_env *env, long long ms);

int			priority_heap_init(t_heap *heap, int capacity);
void		priority_heap_free(t_heap *heap);
void		priority_heap_push(t_heap *heap, long long primary_priority,
				long long secondary_priority, int coder_index);
t_hnode		priority_heap_extract_min(t_heap *heap);
int			priority_heap_is_empty(t_heap *heap);
int			request_has_higher_priority(t_hnode *left, t_hnode *right);
void		swap_heap_nodes(t_hnode *left, t_hnode *right);
void		sift_priority_heap_down(t_heap *heap);

int			request_dongle_pair(t_env *env, int coder_index);
void		release_dongle_pair(t_env *env, int coder_index);

void		*run_coder_thread(void *arg);

void		*run_burnout_monitor(void *arg);

int			parse_simulation_arguments(int argc, char **argv, t_env *env);
int			initialize_simulation_environment(t_env *env);
void		destroy_simulation_environment(t_env *env);

#endif
