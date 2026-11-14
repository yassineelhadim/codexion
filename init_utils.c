/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:34:19 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:34:20 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "init_utils.h"

void	set_dongle_basic_fields(t_dongle *dongle, int id, int capacity)
{
	dongle->id = id;
	dongle->queue_size = 0;
	dongle->queue_capacity = capacity;
	dongle->held_by = -1;
	dongle->available_at = 0;
}

void	set_dongle_comparator(t_dongle *dongle, t_config *config)
{
	if (config->scheduler == FIFO_)
		dongle->cmp = fifo_cmp;
	else
		dongle->cmp = edf_cmp;
}

void	assign_coder_dongles(t_coder *coder, int id, t_config *config)
{
	if (id == 0)
		coder->left_dongle = &config->dongles[config->coder_count - 1];
	else
		coder->left_dongle = &config->dongles[id - 1];
	coder->right_dongle = &config->dongles[id];
}
