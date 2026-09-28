/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_wait.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mecharra <mecharra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/24 14:37:59 by mecharra          #+#    #+#             */
/*   Updated: 2026/09/27 12:19:21 by mecharra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	dongle_request_key(t_sim *sim, int coder_id)
{
	long	key;

	pthread_mutex_lock(&sim->stop_mutex);
	key = sim->coders[coder_id - 1].last_compile_start
		+ sim->cfg.time_to_burnout;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (key);
}
