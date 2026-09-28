/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mecharra <mecharra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 10:18:39 by mecharra          #+#    #+#             */
/*   Updated: 2026/09/27 18:51:33 by mecharra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <limits.h>

typedef enum e_sched
{
	FIFO,
	EDF
}	t_sched;///////////////////////

typedef struct s_config
{
	int		num_coders;
	long	time_to_burnout;
	long	time_to_compile;
	long	time_to_debug;
	long	time_to_refactor;
	int		compiles_required;
	long	dongle_cooldown;
	t_sched	scheduler;
}	t_config;

typedef struct s_sim	t_sim;

# define DONGLE_HEAP_CAP 2

typedef struct s_request
{
	int		coder_id;
	int		count;
	long	key;
}	t_request;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				taken;
	long			cooldown_until;
	t_request		heap[DONGLE_HEAP_CAP];
	int				heap_size;
}	t_dongle;

typedef struct s_coder
{
	pthread_t	thread;
	int			id;
	t_dongle	*left;
	t_dongle	*right;
	long		last_compile_start;
	int			compile_count;
	t_sim		*sim;
}	t_coder;

struct s_sim
{
	t_config		cfg;
	long			start_time;
	int				stop;
	int				burned_out;
	long			fifo_seq;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	start_mutex;
	pthread_cond_t	start_cond;
	int				start_ready;
	int				start_failed;
	pthread_t		monitor;
	t_coder			*coders;
	t_dongle		*dongles;
};

int			parse_args(int argc, char **argv, t_config *cfg);
int			sim_init(t_sim *sim);
int			sim_run(t_sim *sim);
void		sim_cleanup(t_sim *sim);
int			is_number(const char *str);
long		ft_atol(const char *str);
long		time_now_ms(void);
long		sim_elapsed_ms(t_sim *sim);
int			sim_stopped(t_sim *sim);
void		sim_set_stop(t_sim *sim, int burned_out);
void		dongles_destroy(t_dongle *dongles, int n);
int			req_less(t_request a, t_request b);
t_request	heap_pop(t_request *heap, int *size);
t_request	heap_peek(t_request *heap);
void		heap_remove(t_request *heap, int *size, int coder_id);
void		heap_push(t_request *heap, int *size, t_request req);
void		sift_up(t_request *heap, int i);
void		sift_down(t_request *heap, int size, int i);
int			dongle_ready(t_dongle *dongle, t_sim *sim, int coder_id);
long		dongle_request_key(t_sim *sim, int coder_id);
int			dongle_acquire(t_dongle *left, t_dongle *right,
				t_sim *sim, int coder_id);
void		dongle_release(t_dongle *dongle, t_sim *sim);
void		print_log(t_sim *sim, int coder_id, const char *msg);
void		smart_sleep(t_sim *sim, long ms);
int			threads_start(t_sim *sim);
void		threads_join(t_sim *sim);
void		*coder_routine(void *arg);
void		*monitor_routine(void *arg);
void		print_compile_log(t_sim *sim, t_coder *coder);
void		wait_for_start(t_sim *sim);
int			init_fail(t_sim *sim);

#endif