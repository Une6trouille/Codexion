#include "codexion.h"

int	main(int argc, char **argv)
{
	t_shared_data *shared_data;
	t_coder *coders;

	shared_data = malloc(sizeof(t_shared_data));
	if (!parse_args(argc, argv, &shared_data->args))
	{
		free(shared_data);
		return (1);
	}
	init_shared_data(shared_data);
	coders = malloc(sizeof(t_coder) * shared_data->args.nb_coders);
	init_coders(shared_data, coders);

	return (0);
}