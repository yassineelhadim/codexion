/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_priority_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:34:14 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:34:15 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "init_priority_utils.h"

static void	set_priority_even_coder(t_coder *coder)
{
	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		coder->first_dongle = coder->left_dongle;
		coder->second_dongle = coder->right_dongle;
	}
	else
	{
		coder->first_dongle = coder->right_dongle;
		coder->second_dongle = coder->left_dongle;
	}
}

static void	set_priority_odd_coder(t_coder *coder)
{
	if (coder->left_dongle->id > coder->right_dongle->id)
	{
		coder->first_dongle = coder->left_dongle;
		coder->second_dongle = coder->right_dongle;
	}
	else
	{
		coder->first_dongle = coder->right_dongle;
		coder->second_dongle = coder->left_dongle;
	}
}

static void	set_priority_two_coders(t_coder *coder)
{
	if (coder->left_dongle->id > coder->right_dongle->id)
	{
		coder->first_dongle = coder->left_dongle;
		coder->second_dongle = coder->right_dongle;
	}
	else
	{
		coder->first_dongle = coder->right_dongle;
		coder->second_dongle = coder->left_dongle;
	}
}

void	set_coder_dongle_priority(t_coder *coder, t_config *config)
{
	if (config->coder_count == 2)
		set_priority_two_coders(coder);
	else if (coder->id % 2 == 1)
		set_priority_odd_coder(coder);
	else
		set_priority_even_coder(coder);
}
