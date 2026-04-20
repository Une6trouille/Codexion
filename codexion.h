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

typedef struct s_dongle
{
    int free;
    int cooldown;
    int queue;
} t_dongle;

typedef struct s_shared_data
{
    t_args  args;
    t_dongle *dongle;
} t_shared_data;

typedef struct s_coder
{
    int id;
    t_shared_data *t_shared_data;
    int dongle_held;
    int timestamp;
    int nb_compile;
} t_coder;

#include <stdio.h>
#include <string.h>

int parse_args(int argc, char **argv, t_args *args);
static int fill_args(char **argv, t_args *args);
int ft_atoi(char *str);