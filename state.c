/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   state.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mecharra <mecharra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 12:23:58 by mecharra          #+#    #+#             */
/*   Updated: 2026/09/23 12:28:49 by mecharra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	sim_stopped(t_sim *sim)
{
	int	stop;

	pthread_mutex_lock(&sim->stop_mutex);
	stop = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (stop);
}

void	sim_set_stop(t_sim *sim, int burned_out)
{
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = 1;
	if (burned_out)
		sim->burned_out = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
}
