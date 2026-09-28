/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mecharra <mecharra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 14:42:28 by mecharra          #+#    #+#             */
/*   Updated: 2026/09/27 10:15:32 by mecharra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	print_error(const char *msg)
{
	fprintf(stderr, "Error: %s\n", msg);
	return (1);
}

static int	parse_numbers(char **argv, long *nums)
{
	int	i;

	i = 0;
	while (i < 7)
	{
		if (!is_number(argv[i + 1]))
			return (1);
		nums[i] = ft_atol(argv[i + 1]);
		if (nums[i] < 0)
			return (1);
		i++;
	}
	return (0);
}

static int	parse_scheduler(const char *str, t_sched *sched)
{
	if (strcmp(str, "fifo") == 0)
		*sched = FIFO;
	else if (strcmp(str, "edf") == 0)
		*sched = EDF;
	else
		return (1);
	return (0);
}

int	parse_args(int argc, char **argv, t_config *cfg)
{
	long	nums[7];

	if (argc != 9)
		return (print_error("expected 8 arguments"));
	if (parse_numbers(argv, nums) != 0)
		return (print_error("arguments must be positive integers"));
	if (parse_scheduler(argv[8], &cfg->scheduler) != 0)
		return (print_error("scheduler must be 'fifo' or 'edf'"));
	if (nums[0] < 1)
		return (print_error("number_of_coders must be at least 1"));
	cfg->num_coders = (int)nums[0];
	cfg->time_to_burnout = nums[1];
	cfg->time_to_compile = nums[2];
	cfg->time_to_debug = nums[3];
	cfg->time_to_refactor = nums[4];
	cfg->compiles_required = (int)nums[5];
	if (!cfg->compiles_required)
		return (1);
	cfg->dongle_cooldown = nums[6];
	return (0);
}
