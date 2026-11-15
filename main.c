#include "codexion.h"

static void	print_argument_usage(char *name)
{
	fprintf(stderr, "Usage: %s number_of_coders time_to_burnout "
		"time_to_compile time_to_debug time_to_refactor "
		"number_of_compiles_required dongle_cooldown scheduler(fifo|edf)\n"
		"All numeric arguments must be non-negative integers.\n", name);
}

static int	parse_and_initialize(int argc, char **argv, t_env *env)
{
	if (parse_simulation_arguments(argc, argv, env))
	{
		print_argument_usage(argv[0]);
		return (1);
	}
	if (initialize_simulation_environment(env))
	{
		fprintf(stderr, "Error: allocation failed\n");
		return (1);
	}
	gettimeofday(&env->simulation_start_time, NULL);
	return (0);
}

static pthread_t	*launch_coder_threads(t_env *env, pthread_t *monitor)
{
	pthread_t	*coders;
	t_ctx		*c;
	int			i;

	coders = malloc(sizeof(pthread_t) * env->coder_count);
	if (!coders)
		return (NULL);
	pthread_create(monitor, NULL, run_burnout_monitor, env);
	i = 0;
	while (i < env->coder_count)
	{
		c = malloc(sizeof(t_ctx));
		c->env = env;
		c->coder_index = i;
		pthread_create(&coders[i], NULL, run_coder_thread, c);
		i++;
	}
	return (coders);
}

static void	join_simulation_threads(t_env *env, pthread_t *coders,
		pthread_t monitor)
{
	int	i;

	i = 0;
	while (i < env->coder_count)
		pthread_join(coders[i++], NULL);
	pthread_join(monitor, NULL);
}

int	main(int argc, char **argv)
{
	t_env		env;
	pthread_t	monitor;
	pthread_t	*coders;

	if (parse_and_initialize(argc, argv, &env))
		return (1);
	coders = launch_coder_threads(&env, &monitor);
	if (!coders)
	{
		destroy_simulation_environment(&env);
		return (1);
	}
	join_simulation_threads(&env, coders, monitor);
	free(coders);
	destroy_simulation_environment(&env);
	return (0);
}
