/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 00:00:00 by yel-hadi       #+#    #+#             */
/*   Updated: 2026/09/12 00:00:00 by yel-hadi      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Program entry point:
**   1. validate the 8 mandatory arguments into a t_config;
**   2. allocate and initialise every shared resource;
**   3. run the simulation (spawns coders + monitor, waits for everyone);
**   4. release everything (no leaks allowed).
*/
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
