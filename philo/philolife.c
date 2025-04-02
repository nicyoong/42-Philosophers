/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philolife.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 00:01:55 by nyoong            #+#    #+#             */
/*   Updated: 2025/04/03 00:03:16 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	think(t_philosopher *philo)
{
	print_message(philo, "is thinking");
}

void    take_forks(t_philosopher *philo)
{
	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(philo->left_fork);
		print_message(philo, "has taken a fork");
		pthread_mutex_lock(philo->right_fork);
		print_message(philo, "has taken a fork");
	}
	else
	{
		pthread_mutex_lock(philo->right_fork);
		print_message(philo, "has taken a fork");
		precise_usleep(100);
		pthread_mutex_lock(philo->left_fork);
		print_message(philo, "has taken a fork");
	}
}
