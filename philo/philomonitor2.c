/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philomonitor2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 00:43:16 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/22 00:49:57 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	check_and_handle_deaths( t_philosopher *philos, int num_philos,
	t_init_config *config, pthread_t *threads)
{
	int	i;

	i = 0;
	while (i < num_philos)
	{
		if (check_philosopher_status(&philos[i]))
			handle_philosopher_death(&philos[i], config, threads);
		++i;
	}
}

void	check_and_handle_meals(t_philosopher *philos,
	int num_philos, int req_meals, t_init_config *config)
{
	if (req_meals != -1
		&& check_meal_completion(philos, num_philos, req_meals))
	{
		cleanup_resources(config->forks,
			philos, num_philos, config->printf_mutex);
		exit(EXIT_SUCCESS);
	}
}

void	*monitor(void *arg)
{
	t_monitor_args	*args;
	t_philosopher	*philos;
	t_init_config	*config;
	pthread_t		*threads;
	int				req_meals;

	args = (t_monitor_args *)arg;
	philos = args->philosophers;
	config = args->config;
	threads = args->threads;
	req_meals = philos[0].required_meals;
	while (1)
	{
		check_and_handle_deaths(philos, config->num_philos, config, threads);
		check_and_handle_meals(philos, config->num_philos, req_meals, config);
		precise_usleep(100);
	}
	return (NULL);
}
