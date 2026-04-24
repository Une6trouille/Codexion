/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_time.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndi-tull < ndi-tull@student.42lyon.fr >    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 19:21:04 by ndi-tull          #+#    #+#             */
/*   Updated: 2026/04/24 20:10:51 by ndi-tull         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

struct timespec	get_timespec_ms(long ms)
{
	struct timeval tv;
	struct timespec ts;

	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec + (ms / 1000);
	ts.tv_nsec = (tv.tv_usec * 1000) + ((ms % 1000) * 1000000);
	if (ts.tv_nsec >= 1000000000)
	{
		ts.tv_sec += 1;
		ts.tv_nsec -= 1000000000;
	}
	return (ts);
}