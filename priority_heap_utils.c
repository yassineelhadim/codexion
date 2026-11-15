/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/09/07 22:47:09 by mberouak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	request_has_higher_priority(t_hnode *x, t_hnode *y)
{
	if (x->primary_priority != y->primary_priority)
		return (x->primary_priority < y->primary_priority);
	return (x->secondary_priority < y->secondary_priority);
}

void	swap_heap_nodes(t_hnode *x, t_hnode *y)
{
	t_hnode	tmp;

	tmp = *x;
	*x = *y;
	*y = tmp;
}

void	sift_priority_heap_down(t_heap *heap)
{
	int	i;
	int	left;
	int	right;
	int	minimum;

	i = 0;
	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		minimum = i;
		if (left < heap->count && request_has_higher_priority(
				&heap->nodes[left], &heap->nodes[minimum]))
			minimum = left;
		if (right < heap->count && request_has_higher_priority(
				&heap->nodes[right], &heap->nodes[minimum]))
			minimum = right;
		if (minimum == i)
			break ;
		swap_heap_nodes(&heap->nodes[i], &heap->nodes[minimum]);
		i = minimum;
	}
}
