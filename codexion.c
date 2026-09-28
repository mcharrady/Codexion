/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mecharra <mecharra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 10:18:34 by mecharra          #+#    #+#             */
/*   Updated: 2026/09/26 18:04:03 by mecharra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	sim_run(t_sim *sim)
{
	if (threads_start(sim) != 0)
		return (1);
	threads_join(sim);
	return (sim->burned_out);
}

int	main(int argc, char **argv)
{
	t_sim	sim;
	int		ret;

	if (parse_args(argc, argv, &sim.cfg) != 0)
		return (1);
	if (sim_init(&sim) != 0)
		return (1);
	ret = sim_run(&sim);
	sim_cleanup(&sim);
	return (ret);
}
