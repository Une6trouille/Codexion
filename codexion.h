#ifndef CODEXION_H
# define CODEXION_H

#include <stdio.h>
#include <string.h>
#include <pthread.h>

typedef struct s_args
{
    int     nb_coders;
    long    time_to_burnout;
    long    time_to_compile;
    long    time_to_debug;
    long    time_to_refactor;
    int     nb_compiles_required;
    long    dongle_cooldown;
    int     scheduler;
}   t_args;

typedef struct s_queue
{
    int id; // Id du coder en queue
    long timestamp; //combien de temps il lui reste avant de burnout
}t_queue;

typedef struct s_dongle
{
    int free; // Est-ce que le dongle est dispo
    long release_time; // Depuis quand il est dispo
    int nb_in_queue; // combien de coder le veulent
    t_queue *queue; // la queue du dongle
    pthread_mutex_t mutex; // Besoin d'un mutex pour protéger son acces 
    pthread_cond_t condition; // Condition pour savoir juqu'a quand il doit wait
} t_dongle;

typedef struct s_shared_data
{
    t_args  args; // Tous les args parsé 
    t_dongle *dongle; // Tous les dongles
    pthread_mutex_t log_mutex; 
} t_shared_data;

typedef struct s_coder
{
    int id; // Id du coder
    t_shared_data *t_shared_data; // Data parsé et shared avec tout le monde
    int dongle_held; // S'il a un ou plusieurs dongle
    long timestamp; // Depuis quand il a commencé sa derniere compile
    int nb_compile; // nombre de config deja effectué 
} t_coder;

int parse_args(int argc, char **argv, t_args *args);
int ft_atoi(char *str);

#endif