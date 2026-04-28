/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndi-tull <ndi-tull@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 23:44:00 by ndi-tull          #+#    #+#             */
/*   Updated: 2026/04/28 04:13:25 by ndi-tull         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_action(t_coder *coder, char *message)
{
	if (is_simulation_over(coder->t_shared_data))
		return ;
	pthread_mutex_lock(&coder->t_shared_data->log_mutex);
	if (is_simulation_over(coder->t_shared_data))
	{
		pthread_mutex_unlock(&coder->t_shared_data->log_mutex);
		return ;
	}
	printf("%ld %d %s\n", get_time_ms() - coder->t_shared_data->start_time,
		coder->id, message);
	pthread_mutex_unlock(&coder->t_shared_data->log_mutex);
}
