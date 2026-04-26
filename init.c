/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndi-tull <ndi-tull@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 20:24:01 by ndi-tull          #+#    #+#             */
/*   Updated: 2026/04/27 00:55:46 by ndi-tull         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_dongle_queue(t_dongle *dongle, int nb_coders)
{
	dongle->queue.size = 0;
	dongle->queue.data = malloc(sizeof(t_queue) * nb_coders);
	if (!dongle->queue.data)
		return (1);
	return (0);
}

static void	init_dongle_values(t_dongle *dongle)
{
	dongle->free = 1;
	dongle->nb_in_queue = 0;
	dongle->seq_counter = 0;
	dongle->release_time = 0;
	pthread_cond_init(&dongle->condition, NULL);
	pthread_mutex_init(&dongle->mutex, NULL);
}

static int	init_dongles_loop(t_shared_data *shared_data, int i)
{
	while (i < shared_data->args.nb_coders)
	{
		init_dongle_values(&shared_data->dongle[i]);
		if (init_dongle_queue(&shared_data->dongle[i],
				shared_data->args.nb_coders))
		{
			while (i-- > 0)
				free(shared_data->dongle[i].queue.data);
			return (1);
		}
		i++;
	}
	return (0);
}

static int	init_dongles(t_shared_data *shared_data)
{
	shared_data->dongle = malloc(sizeof(t_dongle)
			* shared_data->args.nb_coders);
	if (!shared_data->dongle)
		return (1);
	return (init_dongles_loop(shared_data, 0));
}

void	init_coders(t_shared_data *shared_data, t_coder *coders)
{
	int	i;

	i = 0;
	while (i < shared_data->args.nb_coders)
	{
		coders[i].dongle_held = 0;
		coders[i].id = i + 1;
		coders[i].nb_compile = 0;
		coders[i].timestamp = get_time_ms();
		coders[i].t_shared_data = shared_data;
		pthread_mutex_init(&coders[i].mutex, NULL);
		i++;
	}
}

int	init_shared_data(t_shared_data *shared_data)
{
	shared_data->simulation_over = 0;
	pthread_mutex_init(&shared_data->log_mutex, NULL);
	pthread_mutex_init(&shared_data->simulation_mutex, NULL);
	if (init_dongles(shared_data))
		return (1);
	return (0);
}

void	create_thread(pthread_t *threads, t_coder *coders,
		pthread_t *monitor, t_shared_data *shared_data)
{
	int	i;

	i = 0;
	while (i < shared_data->args.nb_coders)
	{
		pthread_create(&threads[i], NULL, coder_routine, &coders[i]);
		i++;
	}
	pthread_create(monitor, NULL, monitor_routine, shared_data);
}