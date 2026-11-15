/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberouak                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:23:41 by mberouak          #+#    #+#             */
/*   Updated: 2026/09/07 22:47:09 by mberouak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	priority_heap_init(t_heap *h, int cap)
{
	h->nodes = malloc(sizeof(t_hnode) * cap);
	h->count = 0;
	h->capacity = cap;
	return (h->nodes == NULL);
}

void	priority_heap_free(t_heap *h)
{
	free(h->nodes);
}

void	priority_heap_push(t_heap *h, long long primary_priority,
		long long secondary_priority, int id)
{
	int	i;
	int	p;

	i = h->count++;
	h->nodes[i].primary_priority = primary_priority;
	h->nodes[i].secondary_priority = secondary_priority;
	h->nodes[i].coder_index = id;
	while (i > 0)
	{
		p = (i - 1) / 2;
		if (!request_has_higher_priority(&h->nodes[i], &h->nodes[p]))
			break ;
		swap_heap_nodes(&h->nodes[i], &h->nodes[p]);
		i = p;
	}
}

t_hnode	priority_heap_extract_min(t_heap *h)
{
	t_hnode	top;

	top = h->nodes[0];
	h->nodes[0] = h->nodes[--h->count];
	sift_priority_heap_down(h);
	return (top);
}

int	priority_heap_is_empty(t_heap *h)
{
	return (h->count == 0);
}
