/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_routine.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndi-tull <ndi-tull@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 23:51:44 by ndi-tull          #+#    #+#             */
/*   Updated: 2026/04/26 23:52:05 by ndi-tull         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	broadcast_all_dongles(t_shared_data *sd)
{
	int	i;

	i = 0;
	while (i < sd->args.nb_coders)
	{
		pthread_mutex_lock(&sd->dongle[i].mutex);
		pthread_cond_broadcast(&sd->dongle[i].condition);
		pthread_mutex_unlock(&sd->dongle[i].mutex);
		i++;
	}
}

static int	find_burnt_coder(t_shared_data *sd)
{
	int	i;
	int	required;

	i = 0;
	required = sd->args.nb_compiles_required;
	while (i < sd->args.nb_coders)
	{
		pthread_mutex_lock(&sd->coders[i].mutex);
		if (sd->coders[i].nb_compile < required
			&& get_time_ms() - sd->coders[i].timestamp
			> sd->args.time_to_burnout)
		{
			pthread_mutex_unlock(&sd->coders[i].mutex);
			return (i);
		}
		pthread_mutex_unlock(&sd->coders[i].mutex);
		i++;
	}
	return (-1);
}

static int	all_coders_done(t_shared_data *sd)
{
	int	i;

	i = 0;
	while (i < sd->args.nb_coders)
	{
		pthread_mutex_lock(&sd->coders[i].mutex);
		if (sd->coders[i].nb_compile < sd->args.nb_compiles_required)
		{
			pthread_mutex_unlock(&sd->coders[i].mutex);
			return (0);
		}
		pthread_mutex_unlock(&sd->coders[i].mutex);
		i++;
	}
	return (1);
}

static void	stop_simulation(t_shared_data *sd)
{
	pthread_mutex_lock(&sd->simulation_mutex);
	sd->simulation_over = 1;
	pthread_mutex_unlock(&sd->simulation_mutex);
	broadcast_all_dongles(sd);
}

void	*monitor_routine(void *arg)
{
	t_shared_data	*sd;
	int				burned;

	sd = (t_shared_data *)arg;
	while (1)
	{
		burned = find_burnt_coder(sd);
		if (burned != -1)
		{
			stop_simulation(sd);
			log_action(&sd->coders[burned], "burned out");
			return (NULL);
		}
		if (all_coders_done(sd))
		{
			stop_simulation(sd);
			return (NULL);
		}
		broadcast_all_dongles(sd);
		usleep(1000);
	}
	return (NULL);
}
