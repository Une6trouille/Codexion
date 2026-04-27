/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndi-tull <ndi-tull@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 19:01:08 by ndi-tull          #+#    #+#             */
/*   Updated: 2026/04/27 03:55:21 by ndi-tull         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	join_threads(pthread_t *thread, pthread_t *monitor,
		t_shared_data *shared_data)
{
	int	i;

	i = 0;
	while (i < shared_data->args.nb_coders)
	{
		pthread_join(thread[i], NULL);
		i++;
	}
	pthread_join(*monitor, NULL);
}

void	free_all(t_shared_data *shared_data, t_coder *coders,
		pthread_t *threads)
{
	int	i;

	i = 0;
	while (i < shared_data->args.nb_coders)
	{
		pthread_mutex_destroy(&shared_data->dongle[i].mutex);
		pthread_mutex_destroy(&shared_data->coders[i].mutex);
		pthread_cond_destroy(&shared_data->dongle[i].condition);
		free(shared_data->dongle[i].queue.data);
		i++;
	}
	pthread_mutex_destroy(&shared_data->simulation_mutex);
	pthread_mutex_destroy(&shared_data->log_mutex);
	free(shared_data->dongle);
	free(coders);
	free(threads);
	free(shared_data);
}

static int	alloc_main_resources(t_shared_data *sd, t_coder **coders,
		pthread_t **threads)
{
	*coders = malloc(sizeof(t_coder) * sd->args.nb_coders);
	if (!*coders)
		return (1);
	*threads = malloc(sizeof(pthread_t) * sd->args.nb_coders);
	if (!*threads)
	{
		free(*coders);
		*coders = NULL;
		return (1);
	}
	return (0);
}

static void	free_partial(t_shared_data *sd)
{
	destroy_dongles_until(sd->dongle, sd->args.nb_coders);
	pthread_mutex_destroy(&sd->simulation_mutex);
	pthread_mutex_destroy(&sd->log_mutex);
	free(sd->dongle);
	free(sd);
}

int	main(int argc, char **argv)
{
	t_shared_data	*shared_data;
	t_coder			*coders;
	pthread_t		*threads;
	pthread_t		monitor;

	shared_data = malloc(sizeof(t_shared_data));
	if (!shared_data)
		return (1);
	shared_data->coders = NULL;
	shared_data->start_time = get_time_ms();
	if (!parse_args(argc, argv, &shared_data->args))
		return (free(shared_data), 1);
	if (init_shared_data(shared_data))
		return (free(shared_data), 1);
	if (alloc_main_resources(shared_data, &coders, &threads))
		return (free_partial(shared_data), 1);
	if (init_coders(shared_data, coders))
		return (free_partial(shared_data), free(coders), free(threads), 1);
	shared_data->coders = coders;
	if (create_thread(threads, coders, &monitor, shared_data))
		return (free_all(shared_data, coders, threads), 1);
	join_threads(threads, &monitor, shared_data);
	free_all(shared_data, coders, threads);
	return (0);
}