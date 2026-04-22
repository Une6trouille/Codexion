#include "codexion.h"

void	log_action(t_coder *coder, char *message)
{
	pthread_mutex_lock(&coder->t_shared_data->log_mutex);
	printf("%ld %d %s\n", get_time_ms() - coder->t_shared_data->start_time,
		coder->id, message);
	pthread_mutex_unlock(&coder->t_shared_data->log_mutex);
}