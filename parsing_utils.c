/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*                                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <yel-hadi.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 00:00:00 by yel-hadi          #+#    #+#             */
/*   Updated: 2026/11/12 00:00:00 by yel-hadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	parse_decimal_long(const char *str, long *out)
{
	long	value;

	if (str == NULL || *str == '\0')
		return (1);
	value = 0;
	while (*str != '\0')
	{
		if (*str < '0' || *str > '9')
			return (1);
		if (value > (INT_MAX - (*str - '0')) / 10)
			return (1);
		value = value * 10 + (*str - '0');
		str++;
	}
	*out = value;
	return (0);
}

int	parse_positive_long(const char *str, long *out)
{
	if (parse_decimal_long(str, out) != 0 || *out < 1)
		return (1);
	return (0);
}

int	parse_non_negative_long(const char *str, long *out)
{
	if (parse_decimal_long(str, out) != 0)
		return (1);
	return (0);
}

int	parse_scheduler(const char *str, t_scheduler *out)
{
	if (strcmp(str, "fifo") == 0)
		*out = SCHEDULER_FIFO;
	else if (strcmp(str, "edf") == 0)
		*out = SCHEDULER_EDF;
	else
		return (1);
	return (0);
}

void	print_usage(const char *prog)
{
	fprintf(stderr,
		"usage: %s number_of_coders time_to_burnout time_to_compile "
		"time_to_debug time_to_refactor number_of_compiles_required "
		"dongle_cooldown scheduler(fifo|edf)\n", prog);
}
