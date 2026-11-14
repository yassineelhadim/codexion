/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:34:32 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:46:12 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser_utils.h"

static bool	is_valid_digit_char(char c)
{
	return (c >= '0' && c <= '9');
}

static bool	is_valid_number(const char *str)
{
	int			i;
	long long	result;

	i = 0;
	result = 0;
	if (!str || str[0] == '\0')
		return (false);
	while (str[i])
	{
		if (!is_valid_digit_char(str[i]))
			return (false);
		if (result > (INT_MAX - (str[i] - '0')) / 10)
			return (false);
		result = result * 10 + (str[i] - '0');
		i++;
	}
	if (result > INT_MAX)
		return (false);
	return (true);
}

static long long	ms_to_us(const char *str)
{
	return ((long long)atoi(str) * 1000);
}

t_status	validate_numeric_args(char **argv)
{
	int	i;

	i = 1;
	while (i < 8)
	{
		if (!is_valid_number(argv[i]))
			return (FAILURE);
		i++;
	}
	return (SUCCESS);
}

void	assign_config_values(char **argv, t_config *config)
{
	config->coder_count = atoi(argv[1]);
	config->time_to_burnout = ms_to_us(argv[2]);
	config->time_to_compile = ms_to_us(argv[3]);
	config->time_to_debug = ms_to_us(argv[4]);
	config->time_to_refactor = ms_to_us(argv[5]);
	config->compiles_required = atoi(argv[6]);
	config->dongle_cooldown = ms_to_us(argv[7]);
}
