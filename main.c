/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:34:27 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:34:28 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	init_config_defaults(t_config *config)
{
	memset(config, 0, sizeof(t_config));
}

int	main(int argc, char **argv)
{
	t_config	config;

	init_config_defaults(&config);
	if (parse_args(argc, argv, &config) == FAILURE)
	{
		return (1);
	}
	if (init_config(&config) == FAILURE)
	{
		cleanup(&config);
		return (1);
	}
	if (simulate(&config) == FAILURE)
	{
		cleanup(&config);
		return (1);
	}
	cleanup(&config);
	return (0);
}
