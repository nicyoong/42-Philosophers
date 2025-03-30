/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 23:35:00 by nyoong            #+#    #+#             */
/*   Updated: 2025/03/30 23:37:16 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

unsigned long get_current_time() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void print_message(t_philosopher *philo, const char *msg) {
	pthread_mutex_lock(philo->printf_mutex);
	printf("%lu %d %s\n", get_current_time(), philo->id, msg);
	pthread_mutex_unlock(philo->printf_mutex);
}

void *philosopher_life(void *arg) {
	t_philosopher *philo = (t_philosopher *)arg;
	while (1) {
		print_message(philo, "is thinking");
		if (philo->id == philo->total_philosophers) {
			pthread_mutex_lock(philo->right_fork);
			print_message(philo, "has taken a fork");
			pthread_mutex_lock(philo->left_fork);
			print_message(philo, "has taken a fork");
		} else {
			pthread_mutex_lock(philo->left_fork);
			print_message(philo, "has taken a fork");
			pthread_mutex_lock(philo->right_fork);
			print_message(philo, "has taken a fork");
		}
		pthread_mutex_lock(&philo->meal_mutex);
		philo->last_meal_time = get_current_time();
		pthread_mutex_unlock(&philo->meal_mutex);
		print_message(philo, "is eating");
		usleep(philo->time_to_eat * 1000);
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_lock(&philo->meal_mutex);
		philo->meal_count++;
		pthread_mutex_unlock(&philo->meal_mutex);
		print_message(philo, "is sleeping");
		usleep(philo->time_to_sleep * 1000);
	}
	return NULL;
}
