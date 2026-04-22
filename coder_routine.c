#include "codexion.h"

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (coder->nb_compile < coder->t_shared_data->args.nb_compiles_required
		&& coder->t_shared_data->simulation_over == 0)
	{
		take_dongles(coder);
		if (coder->t_shared_data->simulation_over == 1)
			return (NULL);
		coder->timestamp = get_time_ms();
		log_action(coder, "is compiling");
		usleep(coder->t_shared_data->args.time_to_compile * 1000);
		if (coder->t_shared_data->simulation_over == 1)
			return (NULL);
		release_dongles(coder);
		log_action(coder, "is debugging");
		usleep(coder->t_shared_data->args.time_to_debug * 1000);
		if (coder->t_shared_data->simulation_over == 1)
			return (NULL);
		log_action(coder, "is refactoring");
		usleep(coder->t_shared_data->args.time_to_refactor * 1000);
		if (coder->t_shared_data->simulation_over == 1)
			return (NULL);
		coder->nb_compile++;
	}
	return (NULL);
}

void	take_dongles(t_coder *coder)
{
	if (coder->id % 2 == 0)
	{
		take_right_dongle(coder);
		if (coder->t_shared_data->simulation_over == 1)
			return ;
		take_left_dongle(coder);
	}
	else
	{
		take_left_dongle(coder);
		if (coder->t_shared_data->simulation_over == 1)
			return ;
		take_right_dongle(coder);
	}
}

void	take_left_dongle(t_coder *coder)
{
	struct timespec	ts;

	pthread_mutex_lock(&coder->t_shared_data->dongle[coder->id - 1].mutex);
	while ((coder->t_shared_data->dongle[coder->id - 1].free == 0
			|| get_time_ms() - coder->t_shared_data->dongle[coder->id
			- 1].release_time < coder->t_shared_data->args.dongle_cooldown)
		&& coder->t_shared_data->simulation_over == 0)
	{
		ts = get_timespec_ms(coder->t_shared_data->args.dongle_cooldown);
		pthread_cond_timedwait(&coder->t_shared_data->dongle[coder->id
			- 1].condition, &coder->t_shared_data->dongle[coder->id - 1].mutex,
			&ts);
	}
	if (coder->t_shared_data->simulation_over == 1)
	{
		pthread_mutex_unlock(&coder->t_shared_data->dongle[coder->id
			- 1].mutex);
		return ;
	}
	log_action(coder, "has taken a dongle");
	coder->dongle_held += 1;
	coder->t_shared_data->dongle[coder->id - 1].free = 0;
	pthread_mutex_unlock(&coder->t_shared_data->dongle[coder->id - 1].mutex);
}

void	take_right_dongle(t_coder *coder)
{
	struct timespec	ts;

	pthread_mutex_lock(&coder->t_shared_data->dongle[coder->id
		% coder->t_shared_data->args.nb_coders].mutex);
	while ((coder->t_shared_data->dongle[coder->id
			% coder->t_shared_data->args.nb_coders].free == 0 || get_time_ms()
			- coder->t_shared_data->dongle[coder->id
			% coder->t_shared_data->args.nb_coders].release_time < coder->t_shared_data->args.dongle_cooldown)
		&& coder->t_shared_data->simulation_over == 0)
	{
		ts = get_timespec_ms(coder->t_shared_data->args.dongle_cooldown);
		pthread_cond_timedwait(&coder->t_shared_data->dongle[coder->id
			% coder->t_shared_data->args.nb_coders].condition,
			&coder->t_shared_data->dongle[coder->id
			% coder->t_shared_data->args.nb_coders].mutex, &ts);
	}
	if (coder->t_shared_data->simulation_over == 1)
	{
		pthread_mutex_unlock(&coder->t_shared_data->dongle[coder->id
			% coder->t_shared_data->args.nb_coders].mutex);
		return ;
	}
	log_action(coder, "has taken a dongle");
	coder->dongle_held += 1;
	coder->t_shared_data->dongle[coder->id
		% coder->t_shared_data->args.nb_coders].free = 0;
	pthread_mutex_unlock(&coder->t_shared_data->dongle[coder->id
		% coder->t_shared_data->args.nb_coders].mutex);
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