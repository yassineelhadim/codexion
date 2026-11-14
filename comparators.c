/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   comparators.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:33:34 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:33:35 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	fifo_cmp(t_request *a, t_request *b)
{
	return (a->timestamp < b->timestamp);
}

int	edf_cmp(t_request *a, t_request *b)
{
	if (a->deadline == b->deadline)
		return (a->timestamp < b->timestamp);
	return (a->deadline < b->deadline);
}
