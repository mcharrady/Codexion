/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mecharra <mecharra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 13:08:43 by mecharra          #+#    #+#             */
/*   Updated: 2026/09/27 18:52:05 by mecharra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	mutexes_init(t_sim *sim)
{
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->stop_mutex);
		return (1);
	}
	if (pthread_mutex_init(&sim->start_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->stop_mutex);
		pthread_mutex_destroy(&sim->log_mutex);
		return (1);
	}
	return (0);
}

static int	sim_init_fail(t_sim *sim, int dongles_done)
{
	dongles_destroy(sim->dongles, dongles_done);
	free(sim->dongles);
	free(sim->coders);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->start_mutex);
	pthread_cond_destroy(&sim->start_cond);
	return (1);
}

static int	dongles_init(t_dongle *dongles, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		if (pthread_mutex_init(&dongles[i].mutex, NULL) != 0)
			return (i);
		dongles[i].taken = 0;
		dongles[i].cooldown_until = 0;
		dongles[i].heap_size = 0;
		i++;
	}
	return (n);
}

static void	coders_init(t_sim *sim)
{
	int	i;
	int	n;

	n = sim->cfg.num_coders;
	i = 0;
	while (i < n)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].left = &sim->dongles[i];
		sim->coders[i].right = &sim->dongles[(i + 1) % n];
		sim->coders[i].last_compile_start = 0;
		sim->coders[i].compile_count = 0;
		sim->coders[i].sim = sim;
		i++;
	}
}

int	sim_init(t_sim *sim)
{
	int	n;
	int	done;

	sim->coders = NULL;
	sim->dongles = NULL;
	sim->start_ready = 0;
	sim->start_failed = 0;
	n = sim->cfg.num_coders;
	sim->start_time = time_now_ms();
	sim->stop = 0;
	sim->fifo_seq = 0;
	if (mutexes_init(sim) != 0)
		return (1);
	if (pthread_cond_init(&sim->start_cond, NULL))
		return (init_fail(sim));
	sim->dongles = malloc(sizeof(t_dongle) * n);
	sim->coders = malloc(sizeof(t_coder) * n);
	if (!sim->dongles || !sim->coders)
		return (sim_init_fail(sim, 0));
	done = dongles_init(sim->dongles, n);
	if (done != n)
		return (sim_init_fail(sim, done));
	coders_init(sim);
	return (0);
}
