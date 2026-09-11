/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 00:00:00 by yel-hadi       #+#    #+#             */
/*   Updated: 2026/09/11 00:00:00 by yel-hadi      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Strict decimal parser: rejects signs, spaces, non-digit characters,
** empty strings and overflow, unlike atoi() which silently accepts garbage.
** Values are capped at INT_MAX because every numeric field is stored as an
** int or used as a millisecond duration well below that bound.
*/

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

/* Counts need at least one unit: one coder, one required compile. */
int	parse_positive_long(const char *str, long *out)
{
	if (parse_decimal_long(str, out) != 0 || *out < 1)
		return (1);
	return (0);
}

/* Durations in milliseconds may legitimately be zero. */
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
