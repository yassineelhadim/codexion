/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:34:54 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/12 21:13:03 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_us(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long long)tv.tv_sec * 1000000 + tv.tv_usec);
}

long long	get_time_ms(void)
{
	return (get_time_us() / 1000);
}

void	nap(t_config *config, long long us)
{
	long long	target;

	target = get_time_us() + us;
	while (get_sim_state(config) && get_time_us() < target)
		usleep(500);
}
