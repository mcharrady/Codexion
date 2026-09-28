/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mecharra <mecharra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 10:22:03 by mecharra          #+#    #+#             */
/*   Updated: 2026/09/27 18:52:18 by mecharra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_number(const char *str)
{
	int	i;

	if (str[0] == '\0')
		return (0);
	i = 0;
	if (str[i] == '-' || str[i] == '+')
		i++;
	while (str[i] != '\0')
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

long	ft_atol(const char *str)
{
	long	res;
	int		i;
	int		s;

	res = 0;
	i = 0;
	s = 1;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			s = -1;
		i++;
	}
	if (!str[i])
		return (-1);
	while (str[i] >= '0' && str[i] <= '9')
	{
		res = res * 10 + str[i] - '0';
		if (res > INT_MAX)
			return (-1);
		i++;
	}
	return (res * s);
}

void	dongle_release(t_dongle *dongle, t_sim *sim)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->taken = 0;
	dongle->cooldown_until = sim_elapsed_ms(sim) + sim->cfg.dongle_cooldown;
	pthread_mutex_unlock(&dongle->mutex);
}

int	dongle_ready(t_dongle *dongle, t_sim *sim, int coder_id)
{
	if (dongle->taken)
		return (0);
	if (sim_elapsed_ms(sim) < dongle->cooldown_until)
		return (0);
	if (dongle->heap_size == 0)
		return (0);
	return (heap_peek(dongle->heap).coder_id == coder_id);
}

int	init_fail(t_sim *sim)
{
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->start_mutex);
	return (1);
}
