/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philomonitor.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 00:10:11 by nyoong            #+#    #+#             */
/*   Updated: 2025/04/03 00:10:33 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	handle_philosopher_death(t_philosopher *philo)
{
	pthread_mutex_lock(&philo->meal_mutex);
	pthread_mutex_lock(philo->printf_mutex);
	printf("%lu %d died\n", get_current_time() / 1000, philo->id);
	pthread_mutex_unlock(philo->printf_mutex);
	pthread_mutex_unlock(&philo->meal_mutex);
	exit(EXIT_SUCCESS);
}
