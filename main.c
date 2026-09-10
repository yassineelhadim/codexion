/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 20:31:09 by yel-hadi          #+#    #+#             */
/*   Updated: 2026/09/10 20:31:12 by yel-hadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "p_h.h"

int	main(int argc, char **argv)
{
	t_config	config;
	t_table		table;

	if (ft_parser(argc, argv, &config) == -1)
		return (1);
	// ft_print_config(&config);
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
