#include "codexion.h"

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (!should_stop(coder))
	{
		take_dongles(coder);
		pthread_mutex_lock(&coder->t_shared_data->simulation_mutex);
		if (coder->t_shared_data->simulation_over == 1)
		{
			pthread_mutex_unlock(&coder->t_shared_data->simulation_mutex);
			return (NULL);
		}
		pthread_mutex_unlock(&coder->t_shared_data->simulation_mutex);
		pthread_mutex_lock(&coder->mutex);
		coder->timestamp = get_time_ms();
		pthread_mutex_unlock(&coder->mutex);
		log_action(coder, "is compiling");
		usleep(coder->t_shared_data->args.time_to_compile * 1000);
		pthread_mutex_lock(&coder->t_shared_data->simulation_mutex);
		if (coder->t_shared_data->simulation_over == 1)
		{
			pthread_mutex_unlock(&coder->t_shared_data->simulation_mutex);
			return (NULL);
		}
		pthread_mutex_unlock(&coder->t_shared_data->simulation_mutex);
		release_dongles(coder);
		log_action(coder, "is debugging");
		usleep(coder->t_shared_data->args.time_to_debug * 1000);
		pthread_mutex_lock(&coder->t_shared_data->simulation_mutex);
		if (coder->t_shared_data->simulation_over == 1)
		{
			pthread_mutex_unlock(&coder->t_shared_data->simulation_mutex);
			return (NULL);
		}
		pthread_mutex_unlock(&coder->t_shared_data->simulation_mutex);
		log_action(coder, "is refactoring");
		usleep(coder->t_shared_data->args.time_to_refactor * 1000);
		pthread_mutex_lock(&coder->t_shared_data->simulation_mutex);
		if (coder->t_shared_data->simulation_over == 1)
		{
			pthread_mutex_unlock(&coder->t_shared_data->simulation_mutex);
			return (NULL);
		}
		pthread_mutex_unlock(&coder->t_shared_data->simulation_mutex);
		pthread_mutex_lock(&coder->mutex);
		coder->nb_compile++;
		pthread_mutex_unlock(&coder->mutex);
	}
	return (NULL);
}

void	take_dongles(t_coder *coder)
{
	if (coder->id % 2 == 0)
	{
		take_right_dongle(coder);
		pthread_mutex_lock(&coder->t_shared_data->simulation_mutex);
		if (coder->t_shared_data->simulation_over == 1)
		{
			pthread_mutex_unlock(&coder->t_shared_data->simulation_mutex);
			return ;
		}
		pthread_mutex_unlock(&coder->t_shared_data->simulation_mutex);
		take_left_dongle(coder);
	}
	else
	{
		take_left_dongle(coder);
		pthread_mutex_lock(&coder->t_shared_data->simulation_mutex);
		if (coder->t_shared_data->simulation_over == 1)
		{
			pthread_mutex_unlock(&coder->t_shared_data->simulation_mutex);
			return ;
		}
		pthread_mutex_unlock(&coder->t_shared_data->simulation_mutex);
		take_right_dongle(coder);
	}
}

void	take_left_dongle(t_coder *coder)
{
	struct timespec	ts;
	t_queue			entry;
	int				stop;
	int				idx;

	idx = coder->id - 1;
	entry.id = coder->id;
	entry.timestamp = get_time_ms();
	if (coder->t_shared_data->args.scheduler == 1)
		entry.timestamp = coder->timestamp
			+ coder->t_shared_data->args.time_to_burnout;
	pthread_mutex_lock(&coder->t_shared_data->dongle[idx].mutex);
	heap_push(&coder->t_shared_data->dongle[idx].queue, entry,
		coder->t_shared_data->args.scheduler);
	pthread_mutex_lock(&coder->t_shared_data->simulation_mutex);
	stop = coder->t_shared_data->simulation_over;
	pthread_mutex_unlock(&coder->t_shared_data->simulation_mutex);
	while (!stop && (coder->t_shared_data->dongle[idx].free == 0
			|| get_time_ms()
			- coder->t_shared_data->dongle[idx].release_time < coder->t_shared_data->args.dongle_cooldown
			|| coder->t_shared_data->dongle[idx].queue.data[0].id != coder->id))
	{
		ts = get_timespec_ms(coder->t_shared_data->args.dongle_cooldown);
		pthread_cond_timedwait(&coder->t_shared_data->dongle[idx].condition,
			&coder->t_shared_data->dongle[idx].mutex, &ts);
		pthread_mutex_lock(&coder->t_shared_data->simulation_mutex);
		stop = coder->t_shared_data->simulation_over;
		pthread_mutex_unlock(&coder->t_shared_data->simulation_mutex);
	}
	if (stop)
	{
		heap_pop(&coder->t_shared_data->dongle[idx].queue);
		pthread_mutex_unlock(&coder->t_shared_data->dongle[idx].mutex);
		return ;
	}
	heap_pop(&coder->t_shared_data->dongle[idx].queue);
	log_action(coder, "has taken a dongle");
	coder->dongle_held += 1;
	coder->t_shared_data->dongle[idx].free = 0;
	pthread_mutex_unlock(&coder->t_shared_data->dongle[idx].mutex);
}

void	take_right_dongle(t_coder *coder)
{
	struct timespec	ts;
	t_queue			entry;
	int				stop;
	int				idx;

	idx = coder->id % coder->t_shared_data->args.nb_coders;
	entry.id = coder->id;
	entry.timestamp = get_time_ms();
	if (coder->t_shared_data->args.scheduler == 1)
		entry.timestamp = coder->timestamp
			+ coder->t_shared_data->args.time_to_burnout;
	pthread_mutex_lock(&coder->t_shared_data->dongle[idx].mutex);
	heap_push(&coder->t_shared_data->dongle[idx].queue, entry,
		coder->t_shared_data->args.scheduler);
	pthread_mutex_lock(&coder->t_shared_data->simulation_mutex);
	stop = coder->t_shared_data->simulation_over;
	pthread_mutex_unlock(&coder->t_shared_data->simulation_mutex);
	while (!stop && (coder->t_shared_data->dongle[idx].free == 0
			|| get_time_ms()
			- coder->t_shared_data->dongle[idx].release_time < coder->t_shared_data->args.dongle_cooldown
			|| coder->t_shared_data->dongle[idx].queue.data[0].id != coder->id))
	{
		ts = get_timespec_ms(coder->t_shared_data->args.dongle_cooldown);
		pthread_cond_timedwait(&coder->t_shared_data->dongle[idx].condition,
			&coder->t_shared_data->dongle[idx].mutex, &ts);
		pthread_mutex_lock(&coder->t_shared_data->simulation_mutex);
		stop = coder->t_shared_data->simulation_over;
		pthread_mutex_unlock(&coder->t_shared_data->simulation_mutex);
	}
	if (stop)
	{
		heap_pop(&coder->t_shared_data->dongle[idx].queue);
		pthread_mutex_unlock(&coder->t_shared_data->dongle[idx].mutex);
		return ;
	}
	heap_pop(&coder->t_shared_data->dongle[idx].queue);
	log_action(coder, "has taken a dongle");
	coder->dongle_held += 1;
	coder->t_shared_data->dongle[idx].free = 0;
	pthread_mutex_unlock(&coder->t_shared_data->dongle[idx].mutex);
}

void	release_dongles(t_coder *coder)
{
	pthread_mutex_lock(&coder->t_shared_data->dongle[coder->id - 1].mutex);
	coder->t_shared_data->dongle[coder->id - 1].free = 1;
	pthread_cond_broadcast(&coder->t_shared_data->dongle[coder->id
		- 1].condition);
	coder->t_shared_data->dongle[coder->id - 1].release_time = get_time_ms();
	pthread_mutex_unlock(&coder->t_shared_data->dongle[coder->id - 1].mutex);
	pthread_mutex_lock(&coder->t_shared_data->dongle[coder->id
		% coder->t_shared_data->args.nb_coders].mutex);
	coder->t_shared_data->dongle[coder->id
		% coder->t_shared_data->args.nb_coders].free = 1;
	pthread_cond_broadcast(&coder->t_shared_data->dongle[coder->id
		% coder->t_shared_data->args.nb_coders].condition);
	coder->t_shared_data->dongle[coder->id
		% coder->t_shared_data->args.nb_coders].release_time = get_time_ms();
	pthread_mutex_unlock(&coder->t_shared_data->dongle[coder->id
		% coder->t_shared_data->args.nb_coders].mutex);
}

int	should_stop(t_coder *coder)
{
	int stop;

	pthread_mutex_lock(&coder->t_shared_data->simulation_mutex);
	stop = coder->t_shared_data->simulation_over;
	pthread_mutex_unlock(&coder->t_shared_data->simulation_mutex);
	if (stop)
		return (1);
	pthread_mutex_lock(&coder->mutex);
	stop = (coder->nb_compile >= coder->t_shared_data->args.nb_compiles_required);
	pthread_mutex_unlock(&coder->mutex);
	return (stop);
}