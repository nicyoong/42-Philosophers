/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 23:30:52 by nyoong            #+#    #+#             */
/*   Updated: 2025/03/30 23:33:20 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <pthread.h>
# include <sys/time.h>

typedef struct s_philosopher
{
	int				id;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	meal_mutex;
	unsigned long	last_meal_time;
	int				meal_count;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				required_meals;
	pthread_mutex_t	*printf_mutex;
	int				total_philosophers;
}	t_philosopher;

#endif