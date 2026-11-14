/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:34:00 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:34:01 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include "heap_utils.h"

void	heap_push(t_dongle *dongle, t_request req)
{
	if (dongle->queue_size >= dongle->queue_capacity)
		return ;
	dongle->queue[dongle->queue_size] = req;
	dongle->queue_size++;
	heap_up(dongle, dongle->queue_size - 1);
}

t_request	heap_pop(t_dongle *dongle)
{
	t_request	result;

	if (dongle->queue_size == 0)
		return ((t_request){0});
	result = dongle->queue[0];
	dongle->queue[0] = dongle->queue[dongle->queue_size - 1];
	dongle->queue_size--;
	heap_down(dongle, 0);
	return (result);
}

t_request	heap_peek(t_dongle *dongle)
{
	if (dongle->queue_size <= 0)
		return ((t_request){0});
	return (dongle->queue[0]);
}

void	heap_remove(t_dongle *dongle, int coder_id)
{
	int	remove;

	remove = find_coder_in_heap(dongle, coder_id);
	if (remove == -1)
		return ;
	if (remove == dongle->queue_size - 1)
	{
		dongle->queue_size--;
		return ;
	}
	swap_requests(&dongle->queue[remove],
		&dongle->queue[dongle->queue_size - 1]);
	dongle->queue_size--;
	heap_down(dongle, remove);
	heap_up(dongle, remove);
}
