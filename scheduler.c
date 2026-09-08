#include "p_h.h"

void	heap_push(t_heap *heap, t_sched scheduler, t_heap_node new_node)
{
	int			i;
	int			parent;
	t_heap_node	tmp;

	heap->nodes[heap->size] = new_node;
	i = heap->size;
	heap->size++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (heap_has_priority(scheduler, &heap->nodes[i], &heap->nodes[parent]))
		{
			tmp = heap->nodes[i];
			heap->nodes[i] = heap->nodes[parent];
			heap->nodes[parent] = tmp;
			i = parent;
		}
		else
			break ;
	}
}

static int	heap_smallest_child(t_heap *heap, t_sched scheduler, int i)
{
	int	left;
	int	right;
	int	smallest;

	left = i * 2 + 1;
	right = i * 2 + 2;
	smallest = i;
	if (left < heap->size && heap_has_priority(scheduler,
			&heap->nodes[left], &heap->nodes[smallest]))
		smallest = left;
	if (right < heap->size && heap_has_priority(scheduler,
			&heap->nodes[right], &heap->nodes[smallest]))
		smallest = right;
	return (smallest);
}

void	heap_pop_min(t_heap *heap, t_sched scheduler)
{
	int			i;
	int			smallest;
	t_heap_node	tmp;

	heap->size--;
	heap->nodes[0] = heap->nodes[heap->size];
	i = 0;
	while (1)
	{
		smallest = heap_smallest_child(heap, scheduler, i);
		if (smallest == i)
			break ;
		tmp = heap->nodes[i];
		heap->nodes[i] = heap->nodes[smallest];
		heap->nodes[smallest] = tmp;
		i = smallest;
	}
}
