/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mecharra <mecharra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/24 10:57:19 by mecharra          #+#    #+#             */
/*   Updated: 2026/09/27 18:16:34 by mecharra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_log(t_sim *sim, int coder_id, const char *msg)
{
	long	elapsed;

	elapsed = sim_elapsed_ms(sim);
	if (sim_stopped(sim))
		return ;
	printf("%ld %d %s\n", elapsed, coder_id, msg);
}

void	print_compile_log(t_sim *sim, t_coder *coder)
{
	pthread_mutex_lock(&sim->log_mutex);
	if (sim_stopped(sim))
	{
		pthread_mutex_unlock(&sim->log_mutex);
		return ;
	}
	print_log(sim, coder->id, "has taken a dongle");
	print_log(sim, coder->id, "has taken a dongle");
	pthread_mutex_lock(&sim->stop_mutex);
	coder->last_compile_start = sim_elapsed_ms(sim);
	pthread_mutex_unlock(&sim->stop_mutex);
	if (sim_stopped(sim))
	{
		pthread_mutex_unlock(&sim->log_mutex);
		return ;
	}
	print_log(sim, coder->id, "is compiling");
	pthread_mutex_unlock(&sim->log_mutex);
}
