/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndi-tull <ndi-tull@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 19:01:08 by ndi-tull          #+#    #+#             */
/*   Updated: 2026/04/27 05:08:49 by ndi-tull         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	join_threads(t_shared_data *sd, pthread_t *monitor)
{
	int	i;

	i = 0;
	while (i < sd->args.nb_coders)
	{
		pthread_join(sd->threads[i], NULL);
		i++;
	}
	pthread_join(*monitor, NULL);
}

void	free_all(t_shared_data *sd)
{
	int	i;

	i = 0;
	while (i < sd->args.nb_coders)
	{
		pthread_mutex_destroy(&sd->dongle[i].mutex);
		pthread_mutex_destroy(&sd->coders[i].mutex);
		pthread_cond_destroy(&sd->dongle[i].condition);
		free(sd->dongle[i].queue.data);
		i++;
	}
	pthread_mutex_destroy(&sd->simulation_mutex);
	pthread_mutex_destroy(&sd->log_mutex);
	free(sd->dongle);
	free(sd->coders);
	free(sd->threads);
	free(sd);
}

int	main(int argc, char **argv)
{
	t_shared_data	*sd;
	pthread_t		monitor;

	sd = setup_simulation(argc, argv);
	if (!sd)
		return (1);
	if (create_thread(sd->threads, sd->coders, &monitor, sd))
	{
		free_all(sd);
		return (1);
	}
	join_threads(sd, &monitor);
	free_all(sd);
	return (0);
}