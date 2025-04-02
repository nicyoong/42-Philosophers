/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philoutils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 23:47:20 by nyoong            #+#    #+#             */
/*   Updated: 2025/04/03 00:11:54 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_atoi(const char *str)
{
	int	sign;
	int	result;

	sign = 1;
	result = 0;
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		else
			sign = 1;
		str++;
	}
	while (*str >= '0' && *str <= '9')
	{
		result = result * 10 + (*str - '0');
		str++;
	}
	return (result * sign);
}

unsigned long	get_current_time()
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000000 + tv.tv_usec);
}

void	precise_usleep(unsigned long usec)
{
	unsigned long start;

	start = get_current_time();
	while (get_current_time() - start < usec)
		usleep(20);
}

void	print_message(t_philosopher *philo, const char *msg)
{
	unsigned long	time;

	time = get_current_time() / 1000;
	pthread_mutex_lock(philo->printf_mutex);
	printf("%lu %d %s\n", time, philo->id, msg);
	pthread_mutex_unlock(philo->printf_mutex);
}

