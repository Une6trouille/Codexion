/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndi-tull <ndi-tull@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 20:24:01 by ndi-tull          #+#    #+#             */
/*   Updated: 2026/04/28 04:13:15 by ndi-tull         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_dongles(t_shared_data *shared_data)
{
	shared_data->dongle = malloc(sizeof(t_dongle)
			* shared_data->args.nb_coders);
	if (!shared_data->dongle)
		return (1);
	if (init_dongles_loop(shared_data, 0))
	{
		free(shared_data->dongle);
		return (1);
	}
	return (0);
}

int	init_coders(t_shared_data *shared_data, t_coder *coders)
{
	int	i;

	i = 0;
	while (i < shared_data->args.nb_coders)
	{
		coders[i].dongle_held = 0;
		coders[i].id = i + 1;
		coders[i].nb_compile = 0;
		coders[i].timestamp = shared_data->start_time;
		coders[i].t_shared_data = shared_data;
		if (pthread_mutex_init(&coders[i].mutex, NULL) != 0)
		{
			destroy_coders_until(coders, i);
			return (1);
		}
		i++;
	}
	return (0);
}

int	init_shared_data(t_shared_data *shared_data)
{
	shared_data->simulation_over = 0;
	if (pthread_mutex_init(&shared_data->log_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&shared_data->simulation_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&shared_data->log_mutex);
		return (1);
	}
	if (init_dongles(shared_data))
	{
		pthread_mutex_destroy(&shared_data->log_mutex);
		pthread_mutex_destroy(&shared_data->simulation_mutex);
		return (1);
	}
	return (0);
}

int	create_thread(pthread_t *threads, t_coder *coders,
		pthread_t *monitor, t_shared_data *shared_data)
{
	int	i;

	i = 0;
	while (i < shared_data->args.nb_coders)
	{
		if (pthread_create(&threads[i], NULL, coder_routine, &coders[i]) != 0)
		{
			stop_and_join_coders(shared_data, threads, i);
			return (1);
		}
		i++;
	}
	if (pthread_create(monitor, NULL, monitor_routine, shared_data) != 0)
	{
		stop_and_join_coders(shared_data, threads, i);
		return (1);
	}
	return (0);
}
