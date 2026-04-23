#include "codexion.h"

long	ft_atoi(char *str)
{
	long result;
	int i;

	i = 0;
	result = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = (result * 10) + (str[i] - '0');
		i++;
	}
	return (result);
}