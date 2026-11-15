/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/11/15 18:08:30 by mberouak         ###   ########.fr       */
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

# define MODE_FIFO 0
# define MODE_EDF 1

typedef struct s_request
{
	long long	priority;
	long long	order;
	int			coder_id;
}	t_request;

typedef struct s_queue
{
	t_request	*items;
	int			size;
	int			capacity;
}	t_queue;

typedef struct s_dongle
{
	int			in_use;
	long long	ready_at_ms;
}	t_dongle;

typedef struct s_sim
{
	int				coders_count;
	long long		burnout_ms;
	long long		compile_ms;
	long long		debug_ms;
	long long		refactor_ms;
	long long		goal_compiles;
	long long		cooldown_ms;
	int				mode;
	int				stopped;
	long long		next_ticket;
	struct timeval	start_time;
	pthread_mutex_t	lock;
	pthread_mutex_t	print_lock;
	pthread_cond_t	wake_signal;
	t_dongle		*dongles;
	long long		*last_compile_ms;
	long long		*compiles_done;
	int				*is_waiting;
	long long		*req_priority;
	long long		*req_order;
	t_queue			queue;
}	t_sim;

typedef struct s_coder_arg
{
	t_sim	*sim;
	int		coder_id;
}	t_coder_arg;

long long	now_ms(t_sim *sim);
void		set_timeout(struct timespec *ts, t_sim *sim, long long ms);
void		log_status(t_sim *sim, int coder_id, const char *msg);
void		log_status_locked(t_sim *sim, int coder_id, const char *msg);
int			is_stopped(t_sim *sim);
void		smart_sleep(t_sim *sim, long long ms);

int			queue_init(t_queue *q, int capacity);
void		queue_free(t_queue *q);
void		queue_push(t_queue *q, long long priority, long long order,
				int coder_id);
t_request	queue_pop(t_queue *q);
int			queue_empty(t_queue *q);
int			has_higher_priority(t_request *a, t_request *b);
void		swap_requests(t_request *a, t_request *b);
void		queue_sift_down(t_queue *q);

int			take_dongles(t_sim *sim, int coder_id);
void		drop_dongles(t_sim *sim, int coder_id);

void		*coder_loop(void *arg);
void		*monitor_loop(void *arg);

int			parse_args(int argc, char **argv, t_sim *sim);
int			init_sim(t_sim *sim);
void		free_sim(t_sim *sim);

#endif
