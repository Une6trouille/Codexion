/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_init_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndi-tull <ndi-tull@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 01:03:07 by ndi-tull          #+#    #+#             */
/*   Updated: 2026/04/28 04:13:03 by ndi-tull         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_dongle_queue(t_dongle *dongle, int nb_coders)
{
	dongle->queue.size = 0;
	dongle->queue.data = malloc(sizeof(t_queue) * nb_coders);
	if (!dongle->queue.data)
		return (1);
	return (0);
}

int	init_dongle_values(t_dongle *dongle)
{
	dongle->free = 1;
	dongle->nb_in_queue = 0;
	dongle->seq_counter = 0;
	dongle->release_time = 0;
	if (pthread_mutex_init(&dongle->mutex, NULL) != 0)
		return (1);
	if (pthread_cond_init(&dongle->condition, NULL) != 0)
	{
		pthread_mutex_destroy(&dongle->mutex);
		return (1);
	}
	return (0);
}

int	init_dongles_loop(t_shared_data *shared_data, int i)
{
	while (i < shared_data->args.nb_coders)
	{
		if (init_dongle_values(&shared_data->dongle[i]))
		{
			destroy_dongles_until(shared_data->dongle, i);
			return (1);
		}
		if (init_dongle_queue(&shared_data->dongle[i],
				shared_data->args.nb_coders))
		{
			pthread_mutex_destroy(&shared_data->dongle[i].mutex);
			pthread_cond_destroy(&shared_data->dongle[i].condition);
			destroy_dongles_until(shared_data->dongle, i);
			return (1);
		}
		i++;
	}
	return (0);
}
