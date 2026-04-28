/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndi-tull <ndi-tull@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 19:21:13 by ndi-tull          #+#    #+#             */
/*   Updated: 2026/04/28 04:00:59 by ndi-tull         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	queue_less(t_queue a, t_queue b)
{
	if (a.timestamp != b.timestamp)
		return (a.timestamp < b.timestamp);
	return (a.seq < b.seq);
}

static int	sheduler_type(t_heap *heap, int i, int scheduler)
{
	(void)scheduler;
	return (queue_less(heap->data[i], heap->data[(i - 1) / 2]));
}

void	heap_push(t_heap *heap, t_queue element, int scheduler)
{
	int	i;

	heap->data[heap->size] = element;
	heap->size++;
	i = heap->size - 1;
	while (i > 0 && sheduler_type(heap, i, scheduler))
	{
		swap_queue(&heap->data[i], &heap->data[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
}

static int	find_smallest_child(t_heap *heap, int i)
{
	int	left;
	int	right;
	int	smallest;

	left = 2 * i + 1;
	right = 2 * i + 2;
	smallest = i;
	if (left < heap->size
		&& queue_less(heap->data[left], heap->data[smallest]))
		smallest = left;
	if (right < heap->size
		&& queue_less(heap->data[right], heap->data[smallest]))
		smallest = right;
	return (smallest);
}

t_queue	heap_pop(t_heap *heap)
{
	t_queue	top;
	int		i;
	int		smallest;

	top = heap->data[0];
	heap->size--;
	heap->data[0] = heap->data[heap->size];
	i = 0;
	while (1)
	{
		smallest = find_smallest_child(heap, i);
		if (smallest == i)
			break ;
		swap_queue(&heap->data[i], &heap->data[smallest]);
		i = smallest;
	}
	return (top);
}
