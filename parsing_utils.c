#include <string.h>
#include "p_h.h"

/*
** Strict decimal parser: rejects empty strings, non-digit chars
** (including '-', '+', '.', whitespace), and overflow past INT_MAX.
** Refusing atoi()/strtol() on purpose: atoi() gives silent garbage
** on bad input, and we want a hard overflow guard before the value
** ever gets multiplied into a timespec later on.
*/
static int	ft_strict_parse_long(const char *s, long *out)
{
	long	result;

	if (!s || !*s)
		return (-1);
	ft_skip_spaces(&s);
	if (ft_parse_sign(&s))
		return (-1);
	if (!ft_is_digit(*s))
		return (-1);
	result = 0;
	if (ft_parse_digits(&s, &result) == -1)
		return (-1);
	if (*s)
		return (-1);
	*out = result;
	return (0);
}

int	ft_parse_scheduler(const char *s, t_sched *scheduler)
{
	if (strcmp(s, "fifo") == 0)
	{
		*scheduler = SCHED_TYPE_FIFO;
		return (0);
	}
	if (strcmp(s, "edf") == 0)
	{
		*scheduler = SCHED_TYPE_EDF;
		return (0);
	}
	return (-1);
}

int	ft_error(const char *field, const char *reason)
{
	fprintf(stderr, "codexion: invalid %s: %s\n", field, reason);
	return (-1);
}

int	ft_parse_field(const char *arg, const char *name, long *out)
{
	if (ft_strict_parse_long(arg, out) == -1)
		return (ft_error(name, "must be a non-negative integer"));
	return (0);
}
