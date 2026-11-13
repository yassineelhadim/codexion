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

int	main(int argc, char **argv)
{
	t_config	config;
	t_sim		sim;
	int			exit_code;

	if (parse_args(argc, argv, &config) != 0)
		return (1);
	if (sim_init(&sim, &config) != 0)
	{
		fprintf(stderr, "codexion: initialization failed\n");
		sim_destroy(&sim);
		return (1);
	}
	exit_code = sim_run(&sim);
	sim_destroy(&sim);
	return (exit_code);
}
