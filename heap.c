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

int	heap_init(t_heap *h, int cap)
{
	h->a = malloc(sizeof(t_hnode) * cap);
	h->n = 0;
	h->cap = cap;
	return (h->a == NULL);
}

void	heap_free(t_heap *h)
{
	free(h->a);
}

void	heap_push(t_heap *h, long long k1, long long k2, int id)
{
	int	i;
	int	p;

	i = h->n++;
	h->a[i].k1 = k1;
	h->a[i].k2 = k2;
	h->a[i].id = id;
	while (i > 0)
	{
		p = (i - 1) / 2;
		if (!heap_less(&h->a[i], &h->a[p]))
			break ;
		heap_swap(&h->a[i], &h->a[p]);
		i = p;
	}
}

t_hnode	heap_extract_min(t_heap *h)
{
	t_hnode	top;
	int		i;
	int		l;
	int		r;
	int		m;

	top = h->a[0];
	h->a[0] = h->a[--h->n];
	i = 0;
	while (1)
	{
		l = 2 * i + 1;
		r = 2 * i + 2;
		m = i;
		if (l < h->n && heap_less(&h->a[l], &h->a[m]))
			m = l;
		if (r < h->n && heap_less(&h->a[r], &h->a[m]))
			m = r;
		if (m == i)
			break ;
		heap_swap(&h->a[i], &h->a[m]);
		i = m;
	}
	return (top);
}

int	heap_empty(t_heap *h)
{
	return (h->n == 0);
}
