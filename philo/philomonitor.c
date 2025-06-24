/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philomonitor.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 00:10:11 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/23 23:44:14 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

bool	should_stop(t_data *data)
{
	bool	stop;

	pthread_mutex_lock(&data->stop_mutex);
	stop = data->simulation_should_end;
	pthread_mutex_unlock(&data->stop_mutex);
	return (stop);
}

void	handle_philosopher_death(t_philosopher *philo,
	t_init_config *config, t_data *data)
{
	(void)config;
	pthread_mutex_lock(&philo->meal_mutex);
	pthread_mutex_lock(philo->printf_mutex);
	printf("%lu %d died\n", get_current_time() / 1000, philo->id);
	pthread_mutex_unlock(philo->printf_mutex);
	pthread_mutex_unlock(&philo->meal_mutex);
	pthread_mutex_lock(&data->stop_mutex);
	data->simulation_should_end = true;
	pthread_mutex_unlock(&data->stop_mutex);
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
