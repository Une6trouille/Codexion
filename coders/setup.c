/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndi-tull <ndi-tull@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 14:00:00 by ndi-tull          #+#    #+#             */
/*   Updated: 2026/04/27 05:08:21 by ndi-tull         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	alloc_main_resources(t_shared_data *sd)
{
	sd->coders = malloc(sizeof(t_coder) * sd->args.nb_coders);
	if (!sd->coders)
		return (1);
	sd->threads = malloc(sizeof(pthread_t) * sd->args.nb_coders);
	if (!sd->threads)
	{
		free(sd->coders);
		sd->coders = NULL;
		return (1);
	}
	return (0);
}

void	free_partial(t_shared_data *sd)
{
	destroy_dongles_until(sd->dongle, sd->args.nb_coders);
	pthread_mutex_destroy(&sd->simulation_mutex);
	pthread_mutex_destroy(&sd->log_mutex);
	free(sd->dongle);
	free(sd);
}

static int	init_phase_two(t_shared_data *sd)
{
	if (alloc_main_resources(sd))
	{
		free_partial(sd);
		return (1);
	}
	if (init_coders(sd, sd->coders))
	{
		free(sd->coders);
		free(sd->threads);
		sd->coders = NULL;
		free_partial(sd);
		return (1);
	}
	return (0);
}

t_shared_data	*setup_simulation(int argc, char **argv)
{
	t_shared_data	*sd;

	sd = malloc(sizeof(t_shared_data));
	if (!sd)
		return (NULL);
	sd->coders = NULL;
	sd->threads = NULL;
	sd->start_time = get_time_ms();
	if (!parse_args(argc, argv, &sd->args))
	{
		free(sd);
		return (NULL);
	}
	if (init_shared_data(sd))
	{
		free(sd);
		return (NULL);
	}
	if (init_phase_two(sd))
		return (NULL);
	return (sd);
}