/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_ops.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 00:00:00 by yel-hadi       #+#    #+#             */
/*   Updated: 2026/09/11 00:00:00 by yel-hadi      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	sift_down(t_heap *heap, t_scheduler sched, int index)
{
	int	child;
	int	left;
	int	right;

	while (1)
	{
		child = index;
		left = index * 2 + 1;
		right = index * 2 + 2;
		if (left < heap->size
			&& request_wins(sched, &heap->items[left], &heap->items[child]))
			child = left;
		if (right < heap->size
			&& request_wins(sched, &heap->items[right], &heap->items[child]))
			child = right;
		if (child == index)
			return ;
		swap_requests(&heap->items[index], &heap->items[child]);
		index = child;
	}
}

void	heap_pop(t_heap *heap, t_scheduler sched)
{
	if (heap->size == 0)
		return ;
	heap->size--;
	if (heap->size > 0)
	{
		heap->items[0] = heap->items[heap->size];
		sift_down(heap, sched, 0);
	}
}

static void	sift_up(t_heap *heap, t_scheduler sched, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!request_wins(sched, &heap->items[index], &heap->items[parent]))
			return ;
		swap_requests(&heap->items[index], &heap->items[parent]);
		index = parent;
	}
}

void	heap_remove_coder(t_heap *heap, t_scheduler sched, int coder_id)
{
	int	index;

	index = 0;
	while (index < heap->size && heap->items[index].coder_id != coder_id)
		index++;
	if (index == heap->size)
		return ;
	heap->size--;
	if (index == heap->size)
		return ;
	heap->items[index] = heap->items[heap->size];
	if (index > 0 && request_wins(sched, &heap->items[index],
			&heap->items[(index - 1) / 2]))
		sift_up(heap, sched, index);
	else
		sift_down(heap, sched, index);
}
