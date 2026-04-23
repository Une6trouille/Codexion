#include "codexion.h"

void	*monitor_routine(void *arg)
{
	t_shared_data *shared_data;
	int i;
	int all_done;
	int burned;

	shared_data = (t_shared_data *)arg;
	while (1)
	{
		i = 0;
		burned = -1;
		while (i < shared_data->args.nb_coders)
		{
			pthread_mutex_lock(&shared_data->coders[i].mutex);
			if (get_time_ms()
				- shared_data->coders[i].timestamp > shared_data->args.time_to_burnout)
				burned = i;
			pthread_mutex_unlock(&shared_data->coders[i].mutex);
			if (burned != -1)
				break ;
			i++;
		}
		if (burned != -1)
		{
			pthread_mutex_lock(&shared_data->simulation_mutex);
			shared_data->simulation_over = 1;
			pthread_mutex_unlock(&shared_data->simulation_mutex);
			log_action(&shared_data->coders[burned], "burned out");
		}
		all_done = 1;
		i = 0;
		while (i < shared_data->args.nb_coders)
		{
			pthread_mutex_lock(&shared_data->coders[i].mutex);
			if (shared_data->coders[i].nb_compile < shared_data->args.nb_compiles_required)
				all_done = 0;
			pthread_mutex_unlock(&shared_data->coders[i].mutex);
			i++;
		}
		if (all_done)
		{
			pthread_mutex_lock(&shared_data->simulation_mutex);
			shared_data->simulation_over = 1;
			pthread_mutex_unlock(&shared_data->simulation_mutex);
			i = 0;
			while (i < shared_data->args.nb_coders)
			{
				pthread_mutex_lock(&shared_data->dongle[i].mutex);
				pthread_cond_broadcast(&shared_data->dongle[i].condition);
				pthread_mutex_unlock(&shared_data->dongle[i].mutex);
				i++;
			}
			break ;
		}
		pthread_mutex_lock(&shared_data->simulation_mutex);
		if (shared_data->simulation_over == 1)
		{
			pthread_mutex_unlock(&shared_data->simulation_mutex);
			i = 0;
			while (i < shared_data->args.nb_coders)
			{
				pthread_mutex_lock(&shared_data->dongle[i].mutex);
				pthread_cond_broadcast(&shared_data->dongle[i].condition);
				pthread_mutex_unlock(&shared_data->dongle[i].mutex);
				i++;
			}
			break ;
		}
		pthread_mutex_unlock(&shared_data->simulation_mutex);
		usleep(1000);
	}
	return (NULL);
}