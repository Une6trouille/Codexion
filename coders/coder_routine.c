/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndi-tull <ndi-tull@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 19:01:08 by ndi-tull          #+#    #+#             */
/*   Updated: 2026/04/28 02:57:48 by ndi-tull         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	run_phase(t_coder *coder, char *msg, long duration)
{
	long	end;

	log_action(coder, msg);
	end = get_time_ms() + duration;
	while (get_time_ms() < end)
	{
		if (is_simulation_over(coder->t_shared_data))
			return ;
		usleep(1000);
	}
}

static void	work_cycle(t_coder *coder)
{
	pthread_mutex_lock(&coder->mutex);
	coder->timestamp = get_time_ms();
	pthread_mutex_unlock(&coder->mutex);
	run_phase(coder, "is compiling",
		coder->t_shared_data->args.time_to_compile);
	release_dongles(coder);
	if (is_simulation_over(coder->t_shared_data))
		return ;
	run_phase(coder, "is debugging", coder->t_shared_data->args.time_to_debug);
	if (is_simulation_over(coder->t_shared_data))
		return ;
	run_phase(coder, "is refactoring",
		coder->t_shared_data->args.time_to_refactor);
	pthread_mutex_lock(&coder->mutex);
	coder->nb_compile++;
	pthread_mutex_unlock(&coder->mutex);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (!should_stop(coder))
	{
		take_dongles(coder);
		if (is_simulation_over(coder->t_shared_data))
			return (NULL);
		work_cycle(coder);
	}
	return (NULL);
}

void	take_dongles(t_coder *coder)
{
	int	left;
	int	right;

	left = coder->id - 1;
	right = coder->id % coder->t_shared_data->args.nb_coders;
	if (coder->id % 2 == 0)
	{
		take_dongle(coder, right);
		if (is_simulation_over(coder->t_shared_data))
			return ;
		take_dongle(coder, left);
	}
	else
	{
		take_dongle(coder, left);
		if (is_simulation_over(coder->t_shared_data))
			return ;
		take_dongle(coder, right);
	}
}

int	should_stop(t_coder *coder)
{
	int	stop;

	if (is_simulation_over(coder->t_shared_data))
		return (1);
	pthread_mutex_lock(&coder->mutex);
	stop = (coder->nb_compile
			>= coder->t_shared_data->args.nb_compiles_required);
	pthread_mutex_unlock(&coder->mutex);
	return (stop);
}
