/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:34:38 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:34:39 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include "parser_utils.h"

static void	print_usage(void)
{
	fprintf(stderr, "Usage: ./codexion number_of_coders ");
	fprintf(stderr, "time_to_burnout time_to_compile time_to_debug ");
	fprintf(stderr, "time_to_refactor number_of_compiles_required ");
	fprintf(stderr, "dongle_cooldown scheduler\n");
}

static bool	parse_scheduler(const char *str, t_config *config)
{
	if (strcmp(str, "fifo") == 0)
	{
		config->scheduler = FIFO_;
		return (true);
	}
	else if (strcmp(str, "edf") == 0)
	{
		config->scheduler = EDF_;
		return (true);
	}
	return (false);
}

static t_status	parse_numbers(char **argv, t_config *config)
{
	if (validate_numeric_args(argv) == FAILURE)
		return (FAILURE);
	assign_config_values(argv, config);
	return (SUCCESS);
}

static t_status	validate_constraints(t_config *config)
{
	if (config->coder_count <= 0)
		return (FAILURE);
	if (config->compiles_required <= 0)
		return (FAILURE);
	return (SUCCESS);
}

t_status	parse_args(int argc, char **argv, t_config *config)
{
	if (argc != 9)
	{
		print_usage();
		return (FAILURE);
	}
	if (parse_numbers(argv, config) == FAILURE)
	{
		fprintf(stderr, "Error: Invalid numeric argument\n");
		return (FAILURE);
	}
	if (parse_scheduler(argv[8], config) == false)
	{
		fprintf(stderr, "Error: Invalid scheduler ");
		fprintf(stderr, "(must be 'fifo' or 'edf')\n");
		return (FAILURE);
	}
	if (validate_constraints(config) == FAILURE)
	{
		fprintf(stderr, "Error: Invalid parameter values\n");
		return (FAILURE);
	}
	return (SUCCESS);
}
