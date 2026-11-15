/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/11/15 18:08:30 by mberouak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	has_higher_priority(t_request *a, t_request *b)
{
	if (a->priority != b->priority)
		return (a->priority < b->priority);
	return (a->order < b->order);
}

void	swap_requests(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	queue_sift_down(t_queue *q)
{
	int	idx;
	int	left;
	int	right;
	int	best;

	idx = 0;
	while (1)
	{
		left = 2 * idx + 1;
		right = 2 * idx + 2;
		best = idx;
		if (left < q->size
			&& has_higher_priority(&q->items[left], &q->items[best]))
			best = left;
		if (right < q->size
			&& has_higher_priority(&q->items[right], &q->items[best]))
			best = right;
		if (best == idx)
			break ;
		swap_requests(&q->items[idx], &q->items[best]);
		idx = best;
	}
}
