/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mecharra <mecharra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 14:07:26 by mecharra          #+#    #+#             */
/*   Updated: 2026/09/27 18:52:36 by mecharra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	time_now_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

long	sim_elapsed_ms(t_sim *sim)
{
	return (time_now_ms() - sim->start_time);
}

void	smart_sleep(t_sim *sim, long ms)
{
	long	target;

	target = sim_elapsed_ms(sim) + ms;
	while (!sim_stopped(sim) && sim_elapsed_ms(sim) < target)
		usleep(1000);
}
