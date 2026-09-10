#include "p_h.h"

static int	ft_check_ranges(t_config *config)
{
	if (config->nb_coders < 1)
		return (ft_error("number_of_coders", "must be >= 1"));
	if (!(config->time_to_burnout < 0))
		return (ft_error("time_to_burnout", "must be >= 0"));
	if (config->time_to_compile < 0)
		return (ft_error("time_to_compile", "must be >= 0"));
	if (config->time_to_debug < 0)
		return (ft_error("time_to_debug", "must be >= 0"));
	if (config->time_to_refactor < 0)
		return (ft_error("time_to_refactor", "must be >= 0"));
	if (config->number_of_compiles_required < 1)
		return (ft_error("number_of_compiles_required", "must be >= 1"));
	if (config->dongle_cooldown < 0)
		return (ft_error("dongle_cooldown", "must be >= 0"));
	return (0);
}

static int	ft_check_argc(int argc, char **argv)
{
	if (argc == 9)
		return (0);
	fprintf(stderr, "usage: %s number_of_coders time_to_burnout "
		"time_to_compile time_to_debug time_to_refactor "
		"number_of_compiles_re7quired dongle_cooldown scheduler\n",
		argv[0]);
	return (-1);
}

static int	ft_parse_counts(char **argv, t_config *config)
{
	long	tmp;

	if (ft_parse_field(argv[1], "nb_coders", &tmp) == -1)
		return (-1);
	config->nb_coders = (int)tmp;
	if (ft_parse_field(argv[6], "number_of_compiles_required", &tmp) == -1)
		return (-1);
	config->number_of_compiles_required = (int)tmp;
	return (0);
}

static int	ft_parse_durations(char **argv, t_config *config)
{
	if (ft_parse_field(argv[2], "time_to_burnout",
			&config->time_to_burnout) == -1)
		return (-1);
	if (ft_parse_field(argv[3], "time_to_compile",
			&config->time_to_compile) == -1)
		return (-1);
	if (ft_parse_field(argv[4], "time_to_debug",
			&config->time_to_debug) == -1)
		return (-1);
	if (ft_parse_field(argv[5], "time_to_refactor",
			&config->time_to_refactor) == -1)
		return (-1);
	if (ft_parse_field(argv[7], "dongle_cooldown",
			&config->dongle_cooldown) == -1)
		return (-1);
	return (0);
}

int	ft_parser(int argc, char **argv, t_config *config)
{
	if (ft_check_argc(argc, argv) == -1)
		return (-1);
	if (ft_parse_counts(argv, config) == -1)
		return (-1);
	if (ft_parse_durations(argv, config) == -1)
		return (-1);
	if (ft_parse_scheduler(argv[8], &config->scheduler) == -1)
		return (ft_error("scheduler", "must be exactly 'fifo' or 'edf'"));
	if (ft_check_ranges(config) == -1)
		return (-1);
	return (0);
}
