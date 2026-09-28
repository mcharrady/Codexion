/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mecharra <mecharra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 10:34:58 by mecharra          #+#    #+#             */
/*   Updated: 2026/09/26 18:34:20 by mecharra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	threads_start_fail(t_sim *sim, int started, int monitor)
{
	int	i;

	pthread_mutex_lock(&sim->start_mutex);
	sim->start_failed = 1;
	sim->start_ready = 1;
	pthread_cond_broadcast(&sim->start_cond);
	pthread_mutex_unlock(&sim->start_mutex);
	sim_set_stop(sim, 0);
	i = 0;
	while (i < started)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	if (monitor)
		pthread_join(sim->monitor, NULL);
	return (1);
}

int	threads_start(t_sim *sim)
{
	int	i;
	int	n;
	int	monitor;

	n = sim->cfg.num_coders;
	i = 0;
	monitor = 0;
	while (i < n)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]))
			return (threads_start_fail(sim, i, monitor));
		i++;
	}
	if (pthread_create(&sim->monitor, NULL, monitor_routine, sim) != 0)
	{
		monitor = 1;
		return (threads_start_fail(sim, n, monitor));
	}
	pthread_mutex_lock(&sim->start_mutex);
	sim->start_ready = 1;
	pthread_cond_broadcast(&sim->start_cond);
	pthread_mutex_unlock(&sim->start_mutex);
	return (0);
}

void	threads_join(t_sim *sim)
{
	int	i;
	int	n;

	n = sim->cfg.num_coders;
	i = 0;
	while (i < n)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	pthread_join(sim->monitor, NULL);
}
