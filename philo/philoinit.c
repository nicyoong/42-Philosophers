/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philoinit.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 00:15:20 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/23 23:58:05 by nyoong           ###   ########.fr       */
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

void	init_philosopher(t_philosopher *philo,
	int i, t_init_config *config, int argc)
{
	int	required_meals;

	if (argc == 6)
		required_meals = ft_atoi(config->argv[5]);
	else
		required_meals = -1;
	philo->id = i + 1;
	philo->left_fork = &config->forks[i];
	philo->right_fork = &config->forks[(i + 1) % config->num_philos];
	pthread_mutex_init(&philo->meal_mutex, NULL);
	philo->last_meal_time = get_current_time();
	philo->meal_count = 0;
	philo->time_to_die = ft_atoi(config->argv[2]) * 1000;
	philo->time_to_eat = ft_atoi(config->argv[3]) * 1000;
	philo->time_to_sleep = ft_atoi(config->argv[4]) * 1000;
	philo->required_meals = required_meals;
	philo->printf_mutex = config->printf_mutex;
	philo->total_philosophers = config->num_philos;
	philo->config = config;
}

int	initialize_philosophers(t_philosopher **philosophers,
	t_init_config *config, int argc)
{
	int	i;

	*philosophers = malloc(config->num_philos * sizeof(t_philosopher));
	if (!*philosophers)
		return (printf("Error: malloc failed\n"), 1);
	i = 0;
	while (i < config->num_philos)
	{
		init_philosopher(&(*philosophers)[i], i, config, argc);
		i++;
	}
	return (0);
}
