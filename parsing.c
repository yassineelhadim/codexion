/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 00:00:00 by yel-hadi       #+#    #+#             */
/*   Updated: 2026/09/11 00:00:00 by yel-hadi      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** All 8 arguments are mandatory and must be strictly positive
** (time_to_burnout included: a coder that could never start a compile
** would burn out instantly, so 0 is not a useful value either).
** The scheduler string must be exactly "fifo" or "edf".
*/

static int	field_is_invalid(const char *arg, const char *name, long *out,
		int must_be_positive)
{
	int			bad;
	const char	*kind;

	if (must_be_positive)
		bad = parse_positive_long(arg, out);
	else
		bad = parse_non_negative_long(arg, out);
	if (must_be_positive)
		kind = "positive";
	else
		kind = "non-negative";
	if (bad)
	{
		fprintf(stderr, "codexion: %s must be a %s integer, got \"%s\"\n",
			name, kind, arg);
		return (1);
	}
	return (0);
}

static int	parse_numbers(char **argv, t_config *config)
{
	long	nb_coders;
	long	compiles;

	if (field_is_invalid(argv[1], "number_of_coders", &nb_coders, 1))
		return (1);
	config->nb_coders = (int)nb_coders;
	if (field_is_invalid(argv[2], "time_to_burnout",
			&config->time_to_burnout, 0))
		return (1);
	if (field_is_invalid(argv[3], "time_to_compile",
			&config->time_to_compile, 0))
		return (1);
	if (field_is_invalid(argv[4], "time_to_debug", &config->time_to_debug, 0))
		return (1);
	if (field_is_invalid(argv[5], "time_to_refactor",
			&config->time_to_refactor, 0))
		return (1);
	if (field_is_invalid(argv[6], "number_of_compiles_required", &compiles, 1))
		return (1);
	config->nb_compiles_required = (int)compiles;
	if (field_is_invalid(argv[7], "dongle_cooldown",
			&config->dongle_cooldown, 0))
		return (1);
	return (0);
}

int	parse_args(int argc, char **argv, t_config *config)
{
	memset(config, 0, sizeof(*config));
	if (argc != 9)
	{
		fprintf(stderr, "codexion: expected 8 arguments, got %d\n", argc - 1);
		print_usage(argv[0]);
		return (1);
	}
	if (parse_numbers(argv, config) != 0)
		return (1);
	if (parse_scheduler(argv[8], &config->scheduler) != 0)
	{
		fprintf(stderr, "codexion: scheduler must be exactly \"fifo\" or "
			"\"edf\", got \"%s\"\n", argv[8]);
		return (1);
	}
	return (0);
}
