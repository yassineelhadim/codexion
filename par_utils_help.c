#include "p_h.h"
#include <string.h>

int	ft_is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

void	ft_skip_spaces(const char **s)
{
	while (**s == ' ' || (**s >= 9 && **s <= 13))
		(*s)++;
}

int	ft_parse_sign(const char **s)
{
	int	minus;

	minus = 0;
	while (**s == '+' || **s == '-')
	{
		if (**s == '-')
			minus++;
		(*s)++;
	}
	return (minus % 2);
}

int	ft_parse_digits(const char **s, long *result)
{
	while (ft_is_digit(**s))
	{
		if (*result > (INT_MAX - (**s - '0')) / 10)
			return (-1);
		*result = *result * 10 + (**s - '0');
		(*s)++;
	}
	return (0);
}
