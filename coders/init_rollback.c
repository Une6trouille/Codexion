/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_rollback.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndi-tull <ndi-tull@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 12:00:00 by ndi-tull          #+#    #+#             */
/*   Updated: 2026/04/28 04:13:13 by ndi-tull         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_dongles_until(t_dongle *dongles, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		pthread_mutex_destroy(&dongles[i].mutex);
		pthread_cond_destroy(&dongles[i].condition);
		free(dongles[i].queue.data);
		i++;
	}
}

void	destroy_coders_until(t_coder *coders, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		pthread_mutex_destroy(&coders[i].mutex);
		i++;
	}
}

void	stop_and_join_coders(t_shared_data *sd, pthread_t *threads, int n)
{
	int	i;

	pthread_mutex_lock(&sd->simulation_mutex);
	sd->simulation_over = 1;
	pthread_mutex_unlock(&sd->simulation_mutex);
	i = 0;
	while (i < sd->args.nb_coders)
	{
		pthread_mutex_lock(&sd->dongle[i].mutex);
		pthread_cond_broadcast(&sd->dongle[i].condition);
		pthread_mutex_unlock(&sd->dongle[i].mutex);
		i++;
	}
	i = 0;
	while (i < n)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
}
