/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sift.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mecharra <mecharra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 10:23:06 by mecharra          #+#    #+#             */
/*   Updated: 2026/09/27 10:52:15 by mecharra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	req_less(t_request a, t_request b)
{
	if (a.key != b.key)
		return (a.key < b.key);
	if (a.count != b.count)
		return (a.count < b.count);
	return (a.coder_id < b.coder_id);
}

void	sift_up(t_request *heap, int i)
{
	int			parent;
	t_request	tmp;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!req_less(heap[i], heap[parent]))
			break ;
		tmp = heap[parent];
		heap[parent] = heap[i];
		heap[i] = tmp;
		i = parent;
	}
}

void	sift_down(t_request *heap, int size, int i)
{
	t_request	tmp;
	int			left;
	int			right;
	int			smallest;

	while (1)
	{
		left = i * 2 + 1;
		right = i * 2 + 2;
		smallest = i;
		if (left < size && req_less(heap[left], heap[smallest]))
			smallest = left;
		if (right < size && req_less(heap[right], heap[smallest]))
			smallest = right;
		if (smallest == i)
			break ;
		tmp = heap[i];
		heap[i] = heap[smallest];
		heap[smallest] = tmp;
		i = smallest;
	}
}
