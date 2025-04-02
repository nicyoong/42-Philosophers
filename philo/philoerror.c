/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philoerror.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 00:07:17 by nyoong            #+#    #+#             */
/*   Updated: 2025/04/03 00:09:14 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	handle_init_error(pthread_mutex_t *printf_mutex, pthread_mutex_t *forks)
{
	pthread_mutex_destroy(printf_mutex);
	free(forks);
	return (1);
}

int handle_thread_error(pthread_mutex_t *printf_mutex, 
	pthread_mutex_t *forks,	t_philosopher *philosophers, int num_philos)
{
	cleanup_resources(forks, philosophers, num_philos, printf_mutex);
	printf("Error: thread creation failed\n");
	return (1);
}
