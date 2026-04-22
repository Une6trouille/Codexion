#include "codexion.h"

void	*monitor_routine(void *arg)
{
	t_shared_data *shared_data;
	int i;
	int all_done;

	shared_data = (t_shared_data *)arg;
	while (1)
	{
		i = 0;
		while (i < shared_data->args.nb_coders)
		{
			if (get_time_ms()
				- shared_data->coders[i].timestamp > shared_data->args.time_to_burnout)
			{
				shared_data->simulation_over = 1;
				log_action(&shared_data->coders[i], "burned out");
			}
			i++;
		}
		all_done = 1;
		i = 0;
		while (i < shared_data->args.nb_coders)
		{
			if (shared_data->coders[i].nb_compile < shared_data->args.nb_compiles_required)
				all_done = 0;
			i++;
		}
		if (all_done)
			shared_data->simulation_over = 1;

		if (shared_data->simulation_over == 1)
		{
			i = 0;
			while (i < shared_data->args.nb_coders)
			{
				pthread_cond_broadcast(&shared_data->dongle[i].condition);
				i++;
			}
			break ;
		}
		usleep(1000);
	}
	return (NULL);
}