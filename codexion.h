/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndi-tull <ndi-tull@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 19:00:00 by ndi-tull          #+#    #+#             */
/*   Updated: 2026/04/27 03:55:58 by ndi-tull         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <time.h>
# include <unistd.h>

typedef struct s_args
{
	int					nb_coders;
	long				time_to_burnout;
	long				time_to_compile;
	long				time_to_debug;
	long				time_to_refactor;
	int					nb_compiles_required;
	long				dongle_cooldown;
	int					scheduler;
}						t_args;

typedef struct s_queue
{
	int					id;
	long				timestamp;
	long				seq;
}						t_queue;

typedef struct s_heap
{
	t_queue				*data;
	int					size;
}						t_heap;

typedef struct s_dongle
{
	int					free;
	long				release_time;
	long				seq_counter;
	int					nb_in_queue;
	t_heap				queue;
	pthread_mutex_t		mutex;
	pthread_cond_t		condition;
}						t_dongle;

typedef struct s_coder	t_coder;

typedef struct s_shared_data
{
	t_args				args;
	t_dongle			*dongle;
	t_coder				*coders;
	pthread_mutex_t		log_mutex;
	int					simulation_over;
	long				start_time;
	pthread_mutex_t		simulation_mutex;
}						t_shared_data;

typedef struct s_coder
{
	int					id;
	t_shared_data		*t_shared_data;
	int					dongle_held;
	long				timestamp;
	int					nb_compile;
	pthread_mutex_t		mutex;
}						t_coder;

int						parse_args(int argc, char **argv, t_args *args);
long					ft_atoi(char *str);
int						init_coders(t_shared_data *shared_data,
							t_coder *coders);
int						init_shared_data(t_shared_data *shared_data);
long					get_time_ms(void);
int						create_thread(pthread_t *threads, t_coder *coders,
							pthread_t *monitor, t_shared_data *shared_data);
void					*coder_routine(void *arg);
void					take_dongle(t_coder *coder, int idx);
void					take_dongles(t_coder *coder);
int						is_simulation_over(t_shared_data *sd);
struct timespec			get_timespec_ms(long ms);
void					log_action(t_coder *coder, char *message);
void					release_dongles(t_coder *coder);
void					free_all(t_shared_data *shared_data, t_coder *coders,
							pthread_t *threads);
void					*monitor_routine(void *arg);
int						should_stop(t_coder *coder);
void					heap_push(t_heap *heap, t_queue element, int scheduler);
t_queue					heap_pop(t_heap *heap);
void					swap_queue(t_queue *a, t_queue *b);
int						init_dongles_loop(t_shared_data *shared_data, int i);
int						init_dongle_values(t_dongle *dongle);
int						init_dongle_queue(t_dongle *dongle, int nb_coders);
void					destroy_dongles_until(t_dongle *dongles, int n);
void					destroy_coders_until(t_coder *coders, int n);
void					stop_and_join_coders(t_shared_data *sd,
							pthread_t *threads, int n);

#endif