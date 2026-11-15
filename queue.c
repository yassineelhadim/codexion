/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/11/15 18:08:30 by mberouak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	queue_init(t_queue *q, int capacity)
{
	q->items = malloc(sizeof(t_request) * capacity);
	q->size = 0;
	q->capacity = capacity;
	return (q->items == NULL);
}

void	queue_free(t_queue *q)
{
	free(q->items);
}

void	queue_push(t_queue *q, long long priority, long long order,
		int coder_id)
{
	int	idx;
	int	parent;

	idx = q->size++;
	q->items[idx].priority = priority;
	q->items[idx].order = order;
	q->items[idx].coder_id = coder_id;
	while (idx > 0)
	{
		parent = (idx - 1) / 2;
		if (!has_higher_priority(&q->items[idx], &q->items[parent]))
			break ;
		swap_requests(&q->items[idx], &q->items[parent]);
		idx = parent;
	}
}

t_request	queue_pop(t_queue *q)
{
	t_request	top;

	top = q->items[0];
	q->items[0] = q->items[--q->size];
	queue_sift_down(q);
	return (top);
}

int	queue_empty(t_queue *q)
{
	return (q->size == 0);
}
