/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philoinit.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 00:15:20 by nyoong            #+#    #+#             */
/*   Updated: 2025/04/03 00:16:06 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	validate_arguments(int argc, char **argv)
{
	int	num_philos;

	if (argc < 5 || argc > 6)
		return (printf("Error: wrong number of arguments\n"), 1);
	num_philos = ft_atoi(argv[1]);
	if (num_philos <= 0 || ft_atoi(argv[2]) <= 0 || ft_atoi(argv[3]) <= 0
		|| ft_atoi(argv[4]) <= 0 || (argc == 6 && ft_atoi(argv[5]) <= 0))
		return (printf("Error: invalid arguments\n"), 1);
	return (0);
}

int	initialize_forks(pthread_mutex_t **forks, int num_philos)
{
	int	i;

	*forks = malloc(num_philos * sizeof(pthread_mutex_t));
	if (!*forks)
		return (printf("Error: malloc failed\n"), 1);
	i = 0;
	while (i < num_philos)
	{
		pthread_mutex_init(&(*forks)[i], NULL);
		i++;
	}
	return (0);
}
