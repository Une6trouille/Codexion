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
	int id;         // Id du coder en queue
	long timestamp; // combien de temps il lui reste avant de burnout
}						t_queue;

typedef struct s_heap
{
	t_queue *data; // tableau de t_queue
	int size;      // nombre d'éléments actuels
}						t_heap;

typedef struct s_dongle
{
	int free;                 // Est-ce que le dongle est dispo
	long release_time;        // Depuis quand il est dispo
	int nb_in_queue;          // combien de coder le veulent
	t_heap queue;             // la queue du dongle
	pthread_mutex_t mutex;    // Besoin d'un mutex pour protéger son acces
	pthread_cond_t condition; // Condition pour savoir juqu'a quand il doit wait
}						t_dongle;

typedef struct s_coder	t_coder;

typedef struct s_shared_data
{
	t_args args;      // Tous les args parsé
	t_dongle *dongle; // Tous les dongles
	t_coder				*coders;
	pthread_mutex_t log_mutex; // Protection pour ne pas print au meme moment
	int simulation_over;       // Bool pour savoir si la simulation est fini
	long				start_time;
	pthread_mutex_t		simulation_mutex;
}						t_shared_data;

typedef struct s_coder
{
	int id;                       // Id du coder
	t_shared_data *t_shared_data; // Data parsé et shared avec tout le monde
	int dongle_held;              // S'il a un ou plusieurs dongle
	long				timestamp;
	// Depuis quand il a commencé sa derniere compile
	int nb_compile; // nombre de config deja effectué
	pthread_mutex_t		mutex;
}						t_coder;

int						parse_args(int argc, char **argv, t_args *args);
long					ft_atoi(char *str);
void					init_coders(t_shared_data *shared_data,
							t_coder *coders);
int						init_shared_data(t_shared_data *shared_data);
long					get_time_ms(void);
void					create_thread(pthread_t *threads, t_coder *coders,
							pthread_t *monitor, t_shared_data *shared_data);
void					*coder_routine(void *arg);
void					take_right_dongle(t_coder *coder);
void					take_left_dongle(t_coder *coder);
void					take_dongles(t_coder *coder);
struct timespec			get_timespec_ms(long ms);
void					log_action(t_coder *coder, char *message);
void					release_dongles(t_coder *coder);
void					free_all(t_shared_data *shared_data, t_coder *coders,
							pthread_t *threads);
void					*monitor_routine(void *arg);
int						should_stop(t_coder *coder);
void					heap_push(t_heap *heap, t_queue element, int scheduler);
t_queue					heap_pop(t_heap *heap);

#endif