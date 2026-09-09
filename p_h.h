#ifndef PARSING_H
# define PARSING_H

# include <limits.h>
# include <stdlib.h>
# include <pthread.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

typedef struct s_table t_table;

typedef enum e_sched
{
	SCHED_TYPE_FIFO,
	SCHED_TYPE_EDF
}	t_sched;

typedef enum e_state
{
	STATE_WAITING,
	STATE_COMPILING,
	STATE_DEBUGGING,
	STATE_REFACTORING,
	STATE_BURNOUT
}	t_state;

typedef struct s_config
{
	int		nb_coders;
	long	time_to_burnout;
	long	time_to_compile;
	long	time_to_debug;
	long	time_to_refactor;
	int		number_of_compiles_required;
	long	dongle_cooldown;
	t_sched	scheduler;
}	t_config;

typedef struct s_coder
{
	int			coder_id;
	pthread_t	thread;
	pthread_mutex_t	mutex;
	int			left_dongle;
	int			right_dongle;
	long long	last_compile_start;
	int			nb_compiles;
	t_state		state;
	t_table		*table;
}	t_coder;

typedef struct s_heap_node
{
	int			coder_id;
	long long	deadline; // when they die (for EDF)
	long long	arrival_seq; //when they asked (for FIFO)
}	t_heap_node;

typedef struct s_heap
{
	t_heap_node		*nodes;
	int				size;
	int				capacity;
	long long		next_seq;
}	t_heap;

typedef struct s_dongle
{
	int				dongle_id;
	int				availability;
	int				helder;
	long long		time_of_release;
	int				ever_released;
	t_heap			waiting_heap;
	pthread_cond_t	cond;
	pthread_mutex_t	mutex;
}	t_dongle;

typedef struct s_table
{
	t_dongle			*dongles;
	t_coder				*coders;
	t_heap				heap;
	pthread_mutex_t		mutex;
	pthread_cond_t		conduit;
	t_config			config;
	int					stop;
	long long			start_time;
}	t_table;

int			ft_parser(int argc, char **argv, t_config *config);
int			ft_parse_scheduler(const char *s, t_sched *scheduler);
int			ft_error(const char *field, const char *reason);
int			ft_parse_field(const char *arg, const char *name, long *out);
long long	get_time(void);
void		log_locked(t_table *table, int coder_id, char *action);
void		log_action(t_table *table, int coder_id, char *action);
void		*monitor_routine(void *arg);
int			init_table(t_table *table, t_config *config);
int			launch_simulation(t_table *table);
void		take_dongle(t_table *table, int dg_id, int coder_id);
void		release_dongle(t_table *table, int dongle_id);
void		*coder_routine(void *arg);
int			heap_has_priority(t_sched scheduler, t_heap_node *a, t_heap_node *b);
t_heap_node	heap_peek_min(t_heap *heap);
int			heap_is_empty(t_heap *heap);
void		heap_push(t_heap *heap, t_sched scheduler, t_heap_node new_node);
void		heap_pop_min(t_heap *heap, t_sched scheduler);
int			init_dongles(t_table *table);
int			ft_parse_digits(const char **s, long *result);
int			ft_is_digit(char c);
void		ft_skip_spaces(const char **s);
int			ft_parse_sign(const char **s);

#endif
