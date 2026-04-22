#include "codexion.h"

static void	init_dongles(t_shared_data *shared_data)
{
	int	i;

	i = 0;
	shared_data->dongle = malloc(sizeof(t_dongle)
			* shared_data->args.nb_coders);
	while (i < shared_data->args.nb_coders)
	{
		shared_data->dongle[i].free = 1;
		shared_data->dongle[i].nb_in_queue = 0;
		pthread_cond_init(&shared_data->dongle[i].condition, NULL);
		pthread_mutex_init(&shared_data->dongle[i].mutex, NULL);
		shared_data->dongle[i].release_time = get_time_ms();
		shared_data->dongle[i].queue = NULL;
		i++;
	}
}

void	init_coders(t_shared_data *shared_data, t_coder *coders)
{
	int	i;

	i = 0;
	while (i < shared_data->args.nb_coders)
	{
		coders[i].dongle_held = 0;
		coders[i].id = i + 1;
		coders[i].nb_compile = 0;
		coders[i].timestamp = get_time_ms();
		coders[i].t_shared_data = shared_data;
		i++;
	}
}

void	init_shared_data(t_shared_data *shared_data)
{
	shared_data->simulation_over = 0;
	pthread_mutex_init(&shared_data->log_mutex, NULL);
	init_dongles(shared_data);
	shared_data->start_time = get_time_ms();
}

void	create_thread(pthread_t *threads, t_coder *coders, pthread_t *monitor,
		t_shared_data *shared_data)
{
	int i;

	i = 0;
	while (i < shared_data->args.nb_coders)
	{
		pthread_create(&threads[i], NULL, coder_routine, &coders[i]);
		i++;
	}
	pthread_create(monitor, NULL, monitor_routine, shared_data);
}