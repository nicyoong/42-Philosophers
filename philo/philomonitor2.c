/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philomonitor2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 00:43:16 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/27 23:45:25 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static bool	check_and_handle_deaths( t_philosopher *philos, int num_philos,
	t_init_config *config, t_data *data)
{
	int	i;

	i = 0;
	while (i < num_philos)
	{
		if (check_philosopher_status(&philos[i]))
		{
			handle_philosopher_death(&philos[i], config, data);
			return (true);
		}
		++i;
	}
	return (false);
}

bool	check_and_handle_meals(t_philosopher *philos,
	int num_philos, int req_meals, t_data *data)
{
	if (req_meals != -1
		&& check_meal_completion(philos, num_philos, req_meals))
	{
		pthread_mutex_lock(&data->stop_mutex);
		data->simulation_should_end = true;
		pthread_mutex_unlock(&data->stop_mutex);
		return (true);
	}
	return (false);
}

void	*monitor(void *arg)
{
	t_monitor_args	*args;
	t_philosopher	*philos;
	t_init_config	*config;
	t_data			*data;
	int				req_meals;

	args = (t_monitor_args *)arg;
	philos = args->philosophers;
	config = args->config;
	data = args->data;
	req_meals = philos[0].required_meals;
	while (1)
	{
		if (check_and_handle_deaths(philos,
				config->num_philos, config, data))
			return (NULL);
		if (check_and_handle_meals(philos,
				config->num_philos, req_meals, data))
			return (NULL);
		precise_usleep(100);
	}
	return (NULL);
}
