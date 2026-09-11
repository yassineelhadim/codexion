/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 00:00:00 by yel-hadi       #+#    #+#             */
/*   Updated: 2026/09/11 00:00:00 by yel-hadi      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Priority rule for the per-dongle min-heaps:
**   - fifo: the request that arrived first wins;
**   - edf:  the earliest burnout deadline wins, ties broken by arrival
**           order so the policy stays fully deterministic.
** The scheduler chosen on the command line is passed to every heap
** operation, so a run behaves consistently under one policy only.
** A heap entry survives until its request is granted: a coder skipped
** because the dongle was cooling down keeps its queue spot (no starvation).
*/

int	request_wins(t_scheduler sched, const t_request *a, const t_request *b)
{
	if (sched == SCHEDULER_FIFO)
		return (a->arrival_seq < b->arrival_seq);
	if (a->deadline != b->deadline)
		return (a->deadline < b->deadline);
	return (a->arrival_seq < b->arrival_seq);
}

void	swap_requests(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

/* Insert at the bottom, then sift up until the heap order is restored. */
void	heap_push(t_heap *heap, t_scheduler sched, const t_request *request)
{
	int	child;
	int	parent;

	child = heap->size;
	heap->items[child] = *request;
	heap->size++;
	while (child > 0)
	{
		parent = (child - 1) / 2;
		if (!request_wins(sched, &heap->items[child], &heap->items[parent]))
			break ;
		swap_requests(&heap->items[child], &heap->items[parent]);
		child = parent;
	}
}

/* True if coder_id currently owns the highest-priority slot of this queue. */
int	heap_top_is(const t_heap *heap, int coder_id)
{
	if (heap->size == 0)
		return (0);
	return (heap->items[0].coder_id == coder_id);
}
