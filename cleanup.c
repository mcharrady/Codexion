/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mecharra <mecharra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 14:34:21 by mecharra          #+#    #+#             */
/*   Updated: 2026/09/27 12:35:55 by mecharra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	dongles_destroy(t_dongle *dongles, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		pthread_mutex_destroy(&dongles[i].mutex);
		i++;
	}
}

void	sim_cleanup(t_sim *sim)
{
	dongles_destroy(sim->dongles, sim->cfg.num_coders);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->start_mutex);
	pthread_cond_destroy(&sim->start_cond);
	free(sim->dongles);
	free(sim->coders);
}
