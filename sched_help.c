# include "p_h.h"

int	heap_is_empty(t_heap *heap)
{
	return (heap->size == 0);
}

t_heap_node	heap_peek_min(t_heap *heap)
{
	return (heap->nodes[0]);
}

int	heap_has_priority(t_sched scheduler, t_heap_node *a, t_heap_node *b)
{
	if (scheduler == SCHED_TYPE_FIFO)
		return (a->arrival_seq < b->arrival_seq);
	else
		return (a->deadline < b->deadline);
}