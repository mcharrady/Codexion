/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mecharra <mecharra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 09:45:06 by mecharra          #+#    #+#             */
/*   Updated: 2026/09/25 11:55:54 by mecharra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	heap_push(t_request *heap, int *size, t_request req)
{
	int		i;

	heap[*size] = req;
	i = *size;
	(*size)++;
	sift_up(heap, i);
}

t_request	heap_pop(t_request *heap, int *size)
{
	t_request	top;

	top = heap[0];
	(*size)--;
	heap[0] = heap[*size];
	sift_down(heap, *size, 0);
	return (top);
}

t_request	heap_peek(t_request *heap)
{
	return (heap[0]);
}

void	heap_remove(t_request *heap, int *size, int coder_id)
{
	if (*size > 0 && heap[0].coder_id == coder_id)
	{
		if (*size == 2)
			heap[0] = heap[1];
		(*size)--;
	}
	else if (*size == 2 && heap[1].coder_id == coder_id)
		(*size)--;
}
