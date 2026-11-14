/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:33:55 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:33:56 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heap_utils.h"

void	swap_requests(t_request *a, t_request *b)
{
	t_request	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

void	heap_up(t_dongle *dongle, int index)
{
	int	parent;

	if (dongle->queue_size == 0)
		return ;
	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (dongle->cmp(&dongle->queue[index], &dongle->queue[parent]))
		{
			swap_requests(&dongle->queue[index], &dongle->queue[parent]);
			index = parent;
		}
		else
			break ;
	}
}

void	heap_down(t_dongle *dongle, int index)
{
	int	left;
	int	right;
	int	smallest;

	if (dongle->queue_size == 0)
		return ;
	while (1)
	{
		left = 2 * index + 1;
		right = 2 * index + 2;
		smallest = index;
		if (left < dongle->queue_size
			&& dongle->cmp(&dongle->queue[left], &dongle->queue[smallest]))
			smallest = left;
		if (right < dongle->queue_size
			&& dongle->cmp(&dongle->queue[right], &dongle->queue[smallest]))
			smallest = right;
		if (smallest != index)
		{
			swap_requests(&dongle->queue[index], &dongle->queue[smallest]);
			index = smallest;
		}
		else
			break ;
	}
}

int	find_coder_in_heap(t_dongle *dongle, int coder_id)
{
	int	i;

	i = 0;
	while (i < dongle->queue_size)
	{
		if (dongle->queue[i].coder_id == coder_id)
			return (i);
		i++;
	}
	return (-1);
}
