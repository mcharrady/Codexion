/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mecharra <mecharra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 11:14:50 by mecharra          #+#    #+#             */
/*   Updated: 2026/09/27 10:03:42 by mecharra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	wait_for_start(t_sim *sim)
{
	pthread_mutex_lock(&sim->start_mutex);
	while (!sim->start_ready)
		pthread_cond_wait(&sim->start_cond, &sim->start_mutex);
	pthread_mutex_unlock(&sim->start_mutex);
}

static int	one_coder(t_coder *coder, t_sim *sim)
{
	pthread_mutex_lock(&sim->log_mutex);
	if (!sim_stopped(sim))
		printf("%ld %d has taken a dongle\n", sim_elapsed_ms(sim), coder->id);
	pthread_mutex_unlock(&sim->log_mutex);
	while (!sim_stopped(sim))
		usleep(1000);
	return (1);
}

static int	coder_compile(t_coder *coder, t_sim *sim)
{
	if (coder->left == coder->right)
		return (one_coder(coder, sim));
	if (dongle_acquire(coder->left, coder->right, sim, coder->id) != 0)
		return (1);
	print_compile_log(sim, coder);
	if (sim_stopped(sim))
		return (1);
	smart_sleep(sim, sim->cfg.time_to_compile);
	dongle_release(coder->left, sim);
	dongle_release(coder->right, sim);
	pthread_mutex_lock(&sim->stop_mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (0);
}

static int	coder_debug_refactor(t_coder *coder, t_sim *sim)
{
	if (sim_stopped(sim))
		return (1);
	pthread_mutex_lock(&sim->log_mutex);
	print_log(sim, coder->id, "is debugging");
	pthread_mutex_unlock(&sim->log_mutex);
	smart_sleep(sim, sim->cfg.time_to_debug);
	if (sim_stopped(sim))
		return (1);
	pthread_mutex_lock(&sim->log_mutex);
	print_log(sim, coder->id, "is refactoring");
	pthread_mutex_unlock(&sim->log_mutex);
	if (sim_stopped(sim))
		return (1);
	smart_sleep(sim, sim->cfg.time_to_refactor);
	return (0);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_sim	*sim;

	coder = (t_coder *)arg;
	sim = coder->sim;
	wait_for_start(sim);
	if (coder->id % 2 != 0)
		smart_sleep(sim, sim->cfg.time_to_compile / 2);
	else if (sim->cfg.num_coders % 2 != 0
		&& coder->id == sim->cfg.num_coders)
		smart_sleep(sim, sim->cfg.time_to_compile * 2);
	while (!sim_stopped(sim))
	{
		if (coder_compile(coder, sim) != 0)
			break ;
		if (coder_debug_refactor(coder, sim) != 0)
			break ;
	}
	return (NULL);
}
