#include "codexion.h"

static int check_sheduler(char *str, t_args *args)
{
    if (strcmp(str, "fifo") == 0)
        args->scheduler = 0;
    else if (strcmp(str, "edf") == 0)
        args->scheduler = 1;
    else
    {
        fprintf(stderr, "Error, Scheduleur must be fifo or edf\n");
        return (0);
    }
    return (1);

}

static int ft_is_a_valid_int(char *str)
{
    int i;

    i = 0;

    if (!str || str[0] == '\0')
        return (0);
    while (str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            return (0);
        i++;
    }
    return (1);
}

static int check_numeric_args(char **argv)
{
    int i;

    i = 1;
    while (i <= 7)
    {
        if (!ft_is_a_valid_int(argv[i]))
        {
            fprintf(stderr, "Error, args %d must be a positive integer\n", i);
            return (0);
        }
        i ++;
    }
    return (1);
}

static int fill_args(char **argv, t_args *args)
{
    args->nb_coders = ft_atoi(argv[1]);
    args->time_to_burnout = ft_atoi(argv[2]);
    args->time_to_compile = ft_atoi(argv[3]);
    args->time_to_debug = ft_atoi(argv[4]);
    args->time_to_refactor = ft_atoi(argv[5]);
    args->nb_compiles_required = ft_atoi(argv[6]);
    args->dongle_cooldown = ft_atoi(argv[7]);
    if (args->nb_coders < 1)
    {
        fprintf(stderr, "Error, nb_coders must be greater than 0\n");
        return (0); 
    }
    return (check_sheduler(argv[8], args));
}

int parse_args(int argc, char **argv, t_args *args)
{
    if (argc != 9)
    {
        fprintf(stderr, "Usage: ./codexion nb_coders time_to_burnout"
			" time_to_compile time_to_debug time_to_refactor"
			" nb_compiles_required dongle_cooldown scheduler\n");
        return (0);
    }

    if (!check_numeric_args(argv))
        return(0);
    return(fill_args(argv, args));
}