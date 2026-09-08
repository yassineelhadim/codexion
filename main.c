#include "p_h.h"

static void	ft_print_config(t_config *config)
{
	printf("number_of_coders=%d\n", config->nb_coders);
	printf("time_to_burnout=%ld\n", config->time_to_burnout);
	printf("time_to_compile=%ld\n", config->time_to_compile);
	printf("time_to_debug=%ld\n", config->time_to_debug);
	printf("time_to_refactor=%ld\n", config->time_to_refactor);
	printf("number_of_compiles_required=%d\n",
		config->number_of_compiles_required);
	printf("dongle_cooldown=%ld\n", config->dongle_cooldown);
	if (config->scheduler == SCHED_TYPE_FIFO)
		printf("scheduler=fifo\n");
	else
		printf("scheduler=edf\n");
}

int	main(int argc, char **argv)
{
	t_config	config;
	t_table		table;

	if (ft_parser(argc, argv, &config) == -1)
		return (1);
	ft_print_config(&config);
	if (init_table(&table, &config) != 0)
	{
		printf("Error: table initialization failed\n");
		return (1);
	}
	if (launch_simulation(&table) != 0)
	{
		printf("Error: simulation failed\n");
		return (1);
	}
	printf("Test run complete.\n");
	return (0);
}