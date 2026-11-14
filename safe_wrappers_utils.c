/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   safe_wrappers_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:34:41 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:34:42 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "safe_wrappers_utils.h"

t_status	safe_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
	if (pthread_cond_wait(cond, mutex) != 0)
		return (FAILURE);
	return (SUCCESS);
}

t_status	safe_cond_timedwait(pthread_cond_t *cond,
	pthread_mutex_t *mutex, const struct timespec *tv)
{
	if (pthread_cond_timedwait(cond, mutex, tv) != 0)
		return (FAILURE);
	return (SUCCESS);
}
