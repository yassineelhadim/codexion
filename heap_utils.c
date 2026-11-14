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

int	heap_less(t_hnode *x, t_hnode *y)
{
	if (x->k1 != y->k1)
		return (x->k1 < y->k1);
	return (x->k2 < y->k2);
}

void	heap_swap(t_hnode *x, t_hnode *y)
{
	t_hnode	tmp;

	tmp = *x;
	*x = *y;
	*y = tmp;
}
