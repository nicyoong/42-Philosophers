/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philomonitor.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 00:10:11 by nyoong            #+#    #+#             */
/*   Updated: 2025/04/07 21:20:56 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	handle_philosopher_death(t_philosopher *philo,
	t_init_config *config, pthread_t *threads)
{
	pthread_mutex_lock(&philo->meal_mutex);
	pthread_mutex_lock(philo->printf_mutex);
	printf("%lu %d died\n", get_current_time() / 1000, philo->id);
	pthread_mutex_unlock(philo->printf_mutex);
	pthread_mutex_unlock(&philo->meal_mutex);
	cleanup_resources(config->forks, philo, config->num_philos,
		config->printf_mutex);
    free(threads);
	exit(EXIT_SUCCESS);
}

bool	check_philosopher_status(t_philosopher *philo)
{
	bool			starving;
	unsigned long	time_since_meal;
	unsigned long	current_time;

	current_time = get_current_time();
	pthread_mutex_lock(&philo->meal_mutex);
	time_since_meal = current_time - philo->last_meal_time;
	starving = (time_since_meal > philo->time_to_die);
	pthread_mutex_unlock(&philo->meal_mutex);
	return (starving);
}

bool	check_meal_completion(t_philosopher *philos,
	int num_philos, int required)
{
	int	i;

	i = -1;
	while (++i < num_philos)
	{
		pthread_mutex_lock(&philos[i].meal_mutex);
		if (philos[i].meal_count < required)
		{
			pthread_mutex_unlock(&philos[i].meal_mutex);
			return (false);
		}
		pthread_mutex_unlock(&philos[i].meal_mutex);
	}
	return (true);
}

void	*monitor(void *arg)
{
	int				num_philos;
	int				req_meals;
	int				i;

	t_monitor_args *args = (t_monitor_args *)arg;
	t_philosopher *philos = args->philosophers;
    t_init_config *config = args->config;
	pthread_t *threads = args->threads;
	num_philos = philos[0].total_philosophers;
	req_meals = philos[0].required_meals;
	while (1)
	{
		i = -1;
		while (++i < num_philos)
		{
			if (check_philosopher_status(&philos[i]))
				handle_philosopher_death(&philos[i], config, threads);
		}
		if (req_meals != -1
			&& check_meal_completion(philos, num_philos, req_meals))
		{
			cleanup_resources(config->forks, philos, config->num_philos,
				config->printf_mutex);
			exit(EXIT_SUCCESS);
		}
		precise_usleep(100);
	}
	return (NULL);
}
