/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philoutils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 23:47:20 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/24 00:21:17 by nyoong           ###   ########.fr       */
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

int	ft_strcmp(const char *s1, const char *s2)
{
	size_t			i;
	unsigned char	c1;
	unsigned char	c2;

	i = 0;
	while (s1[i] != '\0' || s2[i] != '\0')
	{
		c1 = (unsigned char)s1[i];
		c2 = (unsigned char)s2[i];
		if (c1 != c2)
			return (c1 - c2);
		i++;
	}
	return (0);
}

unsigned long	get_current_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000000 + tv.tv_usec);
}

void	precise_usleep(unsigned long usec)
{
	unsigned long	start;

	start = get_current_time();
	while (get_current_time() - start < usec)
		usleep(20);
}

void	print_message(t_philosopher *philo, const char *msg)
{
	t_data			*data;
	unsigned long	time;

	data = &philo->config->stop_data;
	pthread_mutex_lock(&data->stop_mutex);
	if (data->simulation_should_end && ft_strcmp(msg, "died") != 0)
	{
		pthread_mutex_unlock(&data->stop_mutex);
		return ;
	}
	pthread_mutex_unlock(&data->stop_mutex);
	time = get_current_time() / 1000;
	pthread_mutex_lock(philo->printf_mutex);
	printf("%lu %d %s\n", time, philo->id, msg);
	pthread_mutex_unlock(philo->printf_mutex);
}
