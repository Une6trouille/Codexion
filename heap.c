#include "codexion.h"

static int	sheduler_type(t_heap *heap, int i, int scheduler)
{
	(void)scheduler;
	return (heap->data[(i - 1) / 2].timestamp > heap->data[i].timestamp);
}

static void	swap_queue(t_queue *a, t_queue *b)
{
	t_queue	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
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

t_queue	heap_pop(t_heap *heap)
{
	t_queue top;
	int i;
	int left;
	int right;
	int smallest;

	top = heap->data[0];
	heap->size--;
	heap->data[0] = heap->data[heap->size];
	i = 0;
	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		smallest = i;
		if (left < heap->size
			&& heap->data[left].timestamp < heap->data[smallest].timestamp)
			smallest = left;
		if (right < heap->size
			&& heap->data[right].timestamp < heap->data[smallest].timestamp)
			smallest = right;
		if (smallest == i)
			break ;
		swap_queue(&heap->data[i], &heap->data[smallest]);
		i = smallest;
	}
	return (top);
}