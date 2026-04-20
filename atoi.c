#include <codexion.h>

int ft_atoi(char *str)
{
    int result;
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