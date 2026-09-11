/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_release.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 00:00:00 by yel-hadi       #+#    #+#             */
/*   Updated: 2026/09/11 00:00:00 by yel-hadi      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	put_dongles_on_table(t_sim *sim, t_dongle *left,
		t_dongle *right)
{
	if (right->id == left->id)
		left->hands -= 2;
	else
	{
		left->hands--;
		right->hands--;
	}
	if (left->hands == 0)
	{
		left->holder = -1;
		left->cooldown_until = time_in_ms() + sim->config.dongle_cooldown;
	}
	if (right->id != left->id && right->hands == 0)
	{
		right->holder = -1;
		right->cooldown_until = time_in_ms() + sim->config.dongle_cooldown;
	}
	grant_ready_pairs(sim);
}

void	release_dongles(t_sim *sim, t_coder *coder)
{
	t_dongle	*left;
	t_dongle	*right;

	left = &sim->dongles[coder->left];
	right = &sim->dongles[coder->right];
	pthread_mutex_lock(&sim->lock);
	put_dongles_on_table(sim, left, right);
	pthread_cond_broadcast(&sim->event);
	pthread_mutex_unlock(&sim->lock);
}
