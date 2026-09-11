#include "p_h.h"

int	heap_is_empty(t_heap *heap)
{
	return (heap->size == 0);
}

t_heap_node	heap_peek_min(t_heap *heap)
{
	t_heap_node	empty;

	if (heap->size == 0)
	{
		empty.coder_id = -1;
		empty.arrival_seq = 0;
		empty.deadline = 0;
		return (empty);
	}
	return (heap->nodes[0]);
}

int	heap_has_priority(t_sched scheduler, t_heap_node *a, t_heap_node *b)
{
	if (scheduler == SCHED_TYPE_FIFO)
		return (a->arrival_seq < b->arrival_seq);
	if (a->deadline != b->deadline)
		return (a->deadline < b->deadline);
	return (a->arrival_seq < b->arrival_seq);
}