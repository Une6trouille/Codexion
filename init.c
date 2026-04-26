/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndi-tull <ndi-tull@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 20:24:01 by ndi-tull          #+#    #+#             */
/*   Updated: 2026/04/27 00:03:29 by ndi-tull         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_dongles(t_shared_data *shared_data)
{
	int	i;

	i = 0;
	shared_data->dongle = malloc(sizeof(t_dongle)
			* shared_data->args.nb_coders);
	if (!shared_data->dongle)
		return (1);
	while (i < shared_data->args.nb_coders)
	{
		shared_data->dongle[i].free = 1;
		shared_data->dongle[i].nb_in_queue = 0;
		shared_data->dongle[i].seq_counter = 0;
		pthread_cond_init(&shared_data->dongle[i].condition, NULL);
		pthread_mutex_init(&shared_data->dongle[i].mutex, NULL);
		shared_data->dongle[i].release_time = 0;
		shared_data->dongle[i].queue.size = 0;
		shared_data->dongle[i].queue.data = malloc(sizeof(t_queue)
				* shared_data->args.nb_coders);
		if (!shared_data->dongle[i].queue.data)
		{
			while (i > 0)
			{
				i--;
				free(shared_data->dongle[i].queue.data);
			}
			return (1);
		}
		i++;
	}
	return (0);
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

void	create_thread(pthread_t *threads, t_coder *coders, pthread_t *monitor,
		t_shared_data *shared_data)
{
	int i;

	i = 0;
	while (i < shared_data->args.nb_coders)
	{
		pthread_create(&threads[i], NULL, coder_routine, &coders[i]);
		i++;
	}
	pthread_create(monitor, NULL, monitor_routine, shared_data);
}