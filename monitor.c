/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mecharra <mecharra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 12:32:04 by mecharra          #+#    #+#             */
/*   Updated: 2026/09/27 12:36:57 by mecharra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	monitor_check_burnout(t_sim *sim, int i)
{
	long	elapsed;

	pthread_mutex_lock(&sim->stop_mutex);
	elapsed = sim_elapsed_ms(sim) - sim->coders[i].last_compile_start;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (elapsed >= sim->cfg.time_to_burnout);
}

static int	monitor_all_done(t_sim *sim)
{
	int	i;
	int	n;

	n = sim->cfg.num_coders;
	i = 0;
	while (i < n)
	{
		pthread_mutex_lock(&sim->stop_mutex);
		if (sim->coders[i].compile_count < sim->cfg.compiles_required)
		{
			pthread_mutex_unlock(&sim->stop_mutex);
			return (0);
		}
		pthread_mutex_unlock(&sim->stop_mutex);
		i++;
	}
	return (1);
}

static int	monitor_scan_burnout(t_sim *sim, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		if (monitor_check_burnout(sim, i))
		{
			pthread_mutex_lock(&sim->log_mutex);
			sim_set_stop(sim, 1);
			printf("%ld %d burned out\n", sim_elapsed_ms(sim),
				sim->coders[i].id);
			pthread_mutex_unlock(&sim->log_mutex);
			return (1);
		}
		i++;
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		i;
	int		n;

	sim = (t_sim *)arg;
	wait_for_start(sim);
	n = sim->cfg.num_coders;
	while (!sim_stopped(sim))
	{
		i = 0;
		if (monitor_scan_burnout(sim, n))
			return (NULL);
		if (monitor_all_done(sim))
		{
			sim_set_stop(sim, 0);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
