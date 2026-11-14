/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   safe_wrappers.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysenhaji <ysenhaji@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 18:34:45 by ysenhaji          #+#    #+#             */
/*   Updated: 2026/09/11 18:34:46 by ysenhaji         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_status	safe_mutex_handle(pthread_mutex_t *mutex, t_opcode opcode)
{
	int	ret;

	ret = 0;
	if (opcode == INIT)
		ret = pthread_mutex_init(mutex, NULL);
	else if (opcode == LOCK)
		ret = pthread_mutex_lock(mutex);
	else if (opcode == UNLOCK)
		ret = pthread_mutex_unlock(mutex);
	else if (opcode == DESTROY)
		ret = pthread_mutex_destroy(mutex);
	if (ret != 0)
		return (FAILURE);
	return (SUCCESS);
}

t_status	safe_cond_handle(pthread_cond_t *cond, t_opcode opcode)
{
	int	ret;

	ret = 0;
	if (opcode == INIT)
		ret = pthread_cond_init(cond, NULL);
	else if (opcode == DESTROY)
		ret = pthread_cond_destroy(cond);
	else if (opcode == SIGNAL)
		ret = pthread_cond_signal(cond);
	else if (opcode == BROADCAST)
		ret = pthread_cond_broadcast(cond);
	if (ret != 0)
		return (FAILURE);
	return (SUCCESS);
}

t_status	safe_thread_create(pthread_t *thread,
					void *(*func)(void *), void *arg)
{
	if (pthread_create(thread, NULL, func, arg) != 0)
		return (FAILURE);
	return (SUCCESS);
}

t_status	safe_thread_join(pthread_t thread)
{
	if (pthread_join(thread, NULL) != 0)
		return (FAILURE);
	return (SUCCESS);
}
