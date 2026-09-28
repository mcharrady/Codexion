/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mecharra <mecharra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/24 11:19:43 by mecharra          #+#    #+#             */
/*   Updated: 2026/09/27 12:43:41 by mecharra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	dongles_lock(t_dongle *a, t_dongle *b)
{
	if (a < b)
	{
		pthread_mutex_lock(&a->mutex);
		pthread_mutex_lock(&b->mutex);
	}
	else
	{
		pthread_mutex_lock(&b->mutex);
		pthread_mutex_lock(&a->mutex);
	}
}

static void	dongles_unlock(t_dongle *a, t_dongle *b)
{
	pthread_mutex_unlock(&a->mutex);
	pthread_mutex_unlock(&b->mutex);
}

static void	req_dongle(t_dongle *left, t_dongle *right,
			t_sim *sim, int coder_id)
{
	t_request	req;

	req.coder_id = coder_id;
	pthread_mutex_lock(&sim->stop_mutex);
	req.count = sim->coders[coder_id - 1].compile_count;
	if (sim->cfg.scheduler == FIFO)
		req.key = sim->fifo_seq++;
	pthread_mutex_unlock(&sim->stop_mutex);
	dongles_lock(left, right);
	if (sim->cfg.scheduler == EDF)
		req.key = dongle_request_key(sim, coder_id);
	heap_push(left->heap, &left->heap_size, req);
	heap_push(right->heap, &right->heap_size, req);
	dongles_unlock(left, right);
}

static int	dongles_ready(t_dongle *left, t_dongle *right,
	t_sim *sim, int coder_id)
{
	if (!dongle_ready(left, sim, coder_id))
		return (0);
	if (!dongle_ready(right, sim, coder_id))
		return (0);
	return (1);
}

int	dongle_acquire(t_dongle *left, t_dongle *right, t_sim *sim, int coder_id)
{
	req_dongle(left, right, sim, coder_id);
	while (!sim_stopped(sim))
	{
		dongles_lock(left, right);
		if (dongles_ready(left, right, sim, coder_id))
		{
			heap_pop(left->heap, &left->heap_size);
			heap_pop(right->heap, &right->heap_size);
			left->taken = 1;
			right->taken = 1;
			dongles_unlock(left, right);
			return (0);
		}
		dongles_unlock(left, right);
		usleep(1000);
	}
	dongles_lock(left, right);
	heap_remove(left->heap, &left->heap_size, coder_id);
	heap_remove(right->heap, &right->heap_size, coder_id);
	dongles_unlock(left, right);
	return (1);
}
