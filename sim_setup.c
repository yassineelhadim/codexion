/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_setup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-hadi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 00:00:00 by yel-hadi       #+#    #+#             */
/*   Updated: 2026/09/11 00:00:00 by yel-hadi      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	init_coder(t_coder *coder, int id, int nb_coders)
{
	coder->id = id;
	coder->state = STATE_THINKING;
	coder->left = id - 1;
	coder->right = id % nb_coders;
	coder->deadline = 0;
	coder->nb_compiles = 0;
}

static int	init_dongle(t_dongle *dongle, int id, int nb_coders)
{
	dongle->id = id;
	dongle->holder = -1;
	dongle->hands = 0;
	dongle->cooldown_until = 0;
	dongle->waiting.size = 0;
	dongle->waiting.capacity = nb_coders;
	dongle->waiting.items = malloc(sizeof(t_request) * nb_coders);
	if (dongle->waiting.items == NULL)
		return (1);
	return (0);
}

static int	init_resources(t_sim *sim, const t_config *config)
{
	int	i;

	sim->dongles = malloc(sizeof(t_dongle) * config->nb_coders);
	sim->coders = malloc(sizeof(t_coder) * config->nb_coders);
	if (sim->dongles == NULL || sim->coders == NULL)
		return (1);
	memset(sim->dongles, 0, sizeof(t_dongle) * config->nb_coders);
	memset(sim->coders, 0, sizeof(t_coder) * config->nb_coders);
	i = 0;
	while (i < config->nb_coders)
	{
		init_coder(&sim->coders[i], i + 1, config->nb_coders);
		sim->coders[i].sim = sim;
		sim->coders[i].deadline = sim->start_time
			+ config->time_to_burnout;
		if (init_dongle(&sim->dongles[i], i, config->nb_coders) != 0)
			return (1);
		i++;
	}
	return (0);
}

int	sim_init(t_sim *sim, const t_config *config)
{
	sim->config = *config;
	sim->dongles = NULL;
	sim->coders = NULL;
	sim->lock_ready = 0;
	sim->event_ready = 0;
	sim->stop = 0;
	sim->burnout_id = 0;
	sim->next_arrival = 0;
	sim->start_time = time_in_ms();
	if (pthread_mutex_init(&sim->lock, NULL) != 0)
		return (1);
	sim->lock_ready = 1;
	if (pthread_cond_init(&sim->event, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->lock);
		sim->lock_ready = 0;
		return (1);
	}
	sim->event_ready = 1;
	return (init_resources(sim, config));
}

void	sim_destroy(t_sim *sim)
{
	int	i;

	if (sim->dongles != NULL)
	{
		i = 0;
		while (i < sim->config.nb_coders)
		{
			free(sim->dongles[i].waiting.items);
			i++;
		}
		free(sim->dongles);
		sim->dongles = NULL;
	}
	free(sim->coders);
	sim->coders = NULL;
	if (sim->event_ready)
		pthread_cond_destroy(&sim->event);
	if (sim->lock_ready)
		pthread_mutex_destroy(&sim->lock);
	sim->event_ready = 0;
	sim->lock_ready = 0;
}
