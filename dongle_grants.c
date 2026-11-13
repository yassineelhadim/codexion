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

static int	is_free_and_cooled(t_dongle *dongle)
{
	return (dongle->holder == -1 && time_in_ms() >= dongle->cooldown_until);
}

int	coder_has_both_dongles(t_sim *sim, t_coder *coder)
{
	t_dongle	*left;
	t_dongle	*right;
	int			has_both;

	left = &sim->dongles[coder->left];
	right = &sim->dongles[coder->right];
	lock_dongle_pair(left, right);
	if (left->id == right->id)
		has_both = (left->holder == coder->id && left->hands == 2);
	else
		has_both = (left->holder == coder->id && right->holder == coder->id);
	unlock_dongle_pair(left, right);
	return (has_both);
}

static void	grant_pair(t_sim *sim, t_coder *coder)
{
	t_dongle	*left;
	t_dongle	*right;

	left = &sim->dongles[coder->left];
	right = &sim->dongles[coder->right];
	if (left->id == right->id)
	{
		heap_pop(&left->waiting, sim->config.scheduler);
		left->holder = coder->id;
		left->hands = 2;
	}
	else
	{
		heap_pop(&left->waiting, sim->config.scheduler);
		heap_pop(&right->waiting, sim->config.scheduler);
		left->holder = coder->id;
		right->holder = coder->id;
		left->hands = 1;
		right->hands = 1;
	}
	log_state(sim, coder->id, "has taken a dongle");
	log_state(sim, coder->id, "has taken a dongle");
	coder->state = STATE_COMPILING;
	coder->deadline = time_in_ms() + sim->config.time_to_burnout;
	log_state(sim, coder->id, "is compiling");
}

static int	pair_is_ready(t_sim *sim, t_coder *coder)
{
	t_dongle	*left;
	t_dongle	*right;

	left = &sim->dongles[coder->left];
	right = &sim->dongles[coder->right];
	if (coder->state != STATE_WAITING_DONGLES
		|| left->waiting.size == 0
		|| !heap_top_is(&left->waiting, coder->id))
		return (0);
	if (left->id == right->id)
		return (is_free_and_cooled(left));
	if (right->waiting.size == 0
		|| !heap_top_is(&right->waiting, coder->id))
		return (0);
	if (!is_free_and_cooled(left)
		|| !is_free_and_cooled(right))
		return (0);
	return (1);
}

void	grant_ready_pairs(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.nb_coders)
	{
		lock_dongle_pair(&sim->dongles[sim->coders[i].left],
			&sim->dongles[sim->coders[i].right]);
		if (pair_is_ready(sim, &sim->coders[i]))
			grant_pair(sim, &sim->coders[i]);
		unlock_dongle_pair(&sim->dongles[sim->coders[i].left],
			&sim->dongles[sim->coders[i].right]);
		i++;
	}
}
