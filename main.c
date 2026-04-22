#include "codexion.h"

static void	join_threads(pthread_t *thread, pthread_t *monitor,
		t_shared_data *shared_data)
{
	int	i;

	i = 0;
	while (i < shared_data->args.nb_coders)
	{
		pthread_join(thread[i], NULL);
		i++;
	}
	pthread_join(*monitor, NULL);
}

void	free_all(t_shared_data *shared_data, t_coder *coders,
		pthread_t *threads)
{
	int	i;

	i = 0;
	while (i < shared_data->args.nb_coders)
	{
		pthread_mutex_destroy(&shared_data->dongle[i].mutex);
		pthread_cond_destroy(&shared_data->dongle[i].condition);
		i++;
	}
	pthread_mutex_destroy(&shared_data->log_mutex);
	free(shared_data->dongle);
	free(coders);
	free(threads);
	free(shared_data);
}

int	main(int argc, char **argv)
{
	t_shared_data	*shared_data;
	t_coder			*coders;
	pthread_t		*threads;
	pthread_t		monitor;

	shared_data = malloc(sizeof(t_shared_data));
	shared_data->start_time = get_time_ms();
	if (!parse_args(argc, argv, &shared_data->args))
	{
		free(shared_data);
		return (1);
	}
	init_shared_data(shared_data);
	coders = malloc(sizeof(t_coder) * shared_data->args.nb_coders);
	threads = malloc(sizeof(pthread_t) * shared_data->args.nb_coders);
	init_coders(shared_data, coders);
	create_thread(threads, coders, &monitor, shared_data);
	join_threads(threads, &monitor, shared_data);
	free_all(shared_data, coders, threads);
	return (0);
}
