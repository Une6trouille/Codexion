/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndi-tull <ndi-tull@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 19:01:08 by ndi-tull          #+#    #+#             */
/*   Updated: 2026/04/26 23:42:33 by ndi-tull         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_simulation_over(t_shared_data *sd)
{
	int	over;

	pthread_mutex_lock(&sd->simulation_mutex);
	over = sd->simulation_over;
	pthread_mutex_unlock(&sd->simulation_mutex);
	return (over);
}

static int	dongle_ready(t_coder *coder, int idx)
{
	t_dongle	*d;
	long		cd;

	d = &coder->t_shared_data->dongle[idx];
	cd = coder->t_shared_data->args.dongle_cooldown;
	if (d->free == 0)
		return (0);
	if (get_time_ms() - d->release_time < cd)
		return (0);
	if (d->queue.data[0].id != coder->id)
		return (0);
	return (1);
}

static void	wait_dongle_ready(t_coder *coder, int idx)
{
	t_dongle	*d;

	d = &coder->t_shared_data->dongle[idx];
	while (!is_simulation_over(coder->t_shared_data) && !dongle_ready(coder,
			idx))
		pthread_cond_wait(&d->condition, &d->mutex);
}

void	take_dongle(t_coder *coder, int idx)
{
	t_dongle	*d;
	t_queue		entry;

	d = &coder->t_shared_data->dongle[idx];
	entry.id = coder->id;
	entry.timestamp = get_time_ms();
	if (coder->t_shared_data->args.scheduler == 1)
		entry.timestamp = coder->timestamp
			+ coder->t_shared_data->args.time_to_burnout;
	pthread_mutex_lock(&d->mutex);
	entry.seq = d->seq_counter++;
	heap_push(&d->queue, entry, coder->t_shared_data->args.scheduler);
	wait_dongle_ready(coder, idx);
	heap_pop(&d->queue);
	if (is_simulation_over(coder->t_shared_data))
	{
		pthread_mutex_unlock(&d->mutex);
		return ;
	}
	log_action(coder, "has taken a dongle");
	coder->dongle_held += 1;
	d->free = 0;
	pthread_mutex_unlock(&d->mutex);
}

void	release_dongles(t_coder *coder)
{
	int			left;
	int			right;
	t_dongle	*dongles;

	left = coder->id - 1;
	right = coder->id % coder->t_shared_data->args.nb_coders;
	dongles = coder->t_shared_data->dongle;
	pthread_mutex_lock(&dongles[left].mutex);
	dongles[left].free = 1;
	dongles[left].release_time = get_time_ms();
	pthread_cond_broadcast(&dongles[left].condition);
	pthread_mutex_unlock(&dongles[left].mutex);
	pthread_mutex_lock(&dongles[right].mutex);
	dongles[right].free = 1;
	dongles[right].release_time = get_time_ms();
	pthread_cond_broadcast(&dongles[right].condition);
	pthread_mutex_unlock(&dongles[right].mutex);
}
