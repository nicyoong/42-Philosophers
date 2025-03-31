/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 23:35:00 by nyoong            #+#    #+#             */
/*   Updated: 2025/03/31 20:48:28 by nyoong           ###   ########.fr       */
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
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	print_message(t_philosopher *philo, const char *msg)
{
	pthread_mutex_lock(philo->printf_mutex);
	printf("%lu %d %s\n", get_current_time(), philo->id, msg);
	pthread_mutex_unlock(philo->printf_mutex);
}

void	think(t_philosopher *philo)
{
	print_message(philo, "is thinking");
}

void	take_forks(t_philosopher *philo)
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
		pthread_mutex_lock(philo->left_fork);
		print_message(philo, "has taken a fork");
	}
}

void	update_last_meal(t_philosopher *philo)
{
	pthread_mutex_lock(&philo->meal_mutex);
	philo->last_meal_time = get_current_time();
	pthread_mutex_unlock(&philo->meal_mutex);
}

void	eat(t_philosopher *philo)
{
	print_message(philo, "is eating");
	usleep(philo->time_to_eat * 1000);
}

void	release_forks(t_philosopher *philo)
{
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
}

void	update_meal_count(t_philosopher *philo)
{
	pthread_mutex_lock(&philo->meal_mutex);
	philo->meal_count++;
	pthread_mutex_unlock(&philo->meal_mutex);
}

void	philo_sleep(t_philosopher *philo)
{
	print_message(philo, "is sleeping");
	usleep(philo->time_to_sleep * 1000);
}

void	*philosopher_life(void *arg)
{
	t_philosopher	*philo;

	philo = (t_philosopher *)arg;
	while (1)
	{
		pthread_mutex_lock(&philo->data->stop_mutex);
		if (philo->data->simulation_should_end) {
			pthread_mutex_unlock(&philo->data->stop_mutex);
			break;
		}
		pthread_mutex_unlock(&philo->data->stop_mutex);
		think(philo);
		take_forks(philo);
		update_last_meal(philo);
		eat(philo);
		update_meal_count(philo);
		release_forks(philo);
		philo_sleep(philo);
	}
	return (NULL);
}

void	handle_philosopher_death(t_philosopher *philo)
{
	pthread_mutex_lock(philo->printf_mutex);
	printf("%lu %d died\n", get_current_time(), philo->id);
	exit(EXIT_SUCCESS);
}

bool	check_philosopher_status(t_philosopher *philo, unsigned long current_time)
{
	bool	starving;
	
	pthread_mutex_lock(&philo->meal_mutex);
	starving = (current_time - philo->last_meal_time) >= philo->time_to_die;
	pthread_mutex_unlock(&philo->meal_mutex);
	
	return (starving);
}

bool	check_meal_completion(t_philosopher *philos, int num_philos, int required)
{
	int		i;
	bool	all_ate;

	all_ate = true;
	i = 0;
	while (i < num_philos)
	{
		pthread_mutex_lock(&philos[i].meal_mutex);
		if (philos[i].meal_count < required)
			all_ate = false;
		pthread_mutex_unlock(&philos[i].meal_mutex);
		i++;
	}
	return (all_ate);
}

void *monitor(void *arg)
{
	t_philosopher *philos = (t_philosopher *)arg;
	t_data *data = philos[0].data;

	while (1) {
		unsigned long current_time = get_current_time();
		for (int i = 0; i < philos[0].total_philosophers; i++) {
			if (check_philosopher_status(&philos[i], current_time)) {
				pthread_mutex_lock(&data->stop_mutex);
				data->simulation_should_end = true;
				pthread_mutex_unlock(&data->stop_mutex);
				handle_philosopher_death(&philos[i]);
				return NULL;
			}
		}
		if (req_meals != -1 && check_meal_completion(philos, num_philos, req_meals)) {
			pthread_mutex_lock(&data->stop_mutex);
			data->simulation_should_end = true;
			pthread_mutex_unlock(&data->stop_mutex);
			return NULL;
		}
		usleep(1000);
	}
	return NULL;
}

//It still eats after fulfilling all conditions that philos must eat at least n times and then immediately stop

int	main(int argc, char **argv)
{
	int				num_philos;
	t_philosopher	*philosophers;
	pthread_mutex_t	*forks;
	pthread_mutex_t	printf_mutex;
	pthread_t		*threads;
	pthread_t		monitor_thread;
	int				i;
	t_data			control_data;

	pthread_mutex_init(&control_data.stop_mutex, NULL);
	control_data.simulation_should_end = false;
	if (argc < 5 || argc > 6)
		return (printf("Error: wrong number of arguments\n"), 1);
	num_philos = ft_atoi(argv[1]);
	if (num_philos <= 0 || ft_atoi(argv[2]) <= 0 || ft_atoi(argv[3]) <= 0
		|| ft_atoi(argv[4]) <= 0 || (argc == 6 && ft_atoi(argv[5]) <= 0))
		return (printf("Error: invalid arguments\n"), 1);
	forks = malloc(num_philos * sizeof(pthread_mutex_t));
	if (!forks)
		return (printf("Error: malloc failed\n"), 1);
	for (i = 0; i < num_philos; i++)
		pthread_mutex_init(&forks[i], NULL);
	pthread_mutex_init(&printf_mutex, NULL);
	philosophers = malloc(num_philos * sizeof(t_philosopher));
	if (!philosophers)
		return (free(forks), printf("Error: malloc failed\n"), 1);
	int required_meals = argc == 6 ? ft_atoi(argv[5]) : -1;
	for (i = 0; i < num_philos; i++)
	{
		philosophers[i].id = i + 1;
		philosophers[i].left_fork = &forks[i];
		philosophers[i].right_fork = &forks[(i + 1) % num_philos];
		pthread_mutex_init(&philosophers[i].meal_mutex, NULL);
		philosophers[i].last_meal_time = get_current_time();
		philosophers[i].meal_count = 0;
		philosophers[i].time_to_die = ft_atoi(argv[2]);
		philosophers[i].time_to_eat = ft_atoi(argv[3]);
		philosophers[i].time_to_sleep = ft_atoi(argv[4]);
		philosophers[i].required_meals = required_meals;
		philosophers[i].printf_mutex = &printf_mutex;
		philosophers[i].total_philosophers = num_philos;
		philosophers[i].data = &control_data;
	}
	if (pthread_create(&monitor_thread, NULL, monitor, philosophers) != 0)
		return (free(forks), free(philosophers), printf("Error: thread creation failed\n"), 1);
	pthread_detach(monitor_thread);
	threads = malloc(num_philos * sizeof(pthread_t));
	if (!threads)
		return (free(forks), free(philosophers), printf("Error: malloc failed\n"), 1);
	for (i = 0; i < num_philos; i++)
	{
		if (pthread_create(&threads[i], NULL, philosopher_life, &philosophers[i]) != 0)
			return (free(forks), free(philosophers), free(threads), printf("Error: thread creation failed\n"), 1);
		usleep(100);
	}
	for (i = 0; i < num_philos; i++)
		pthread_join(threads[i], NULL);
	free(threads);
	for (i = 0; i < num_philos; i++)
	{
		pthread_mutex_destroy(&forks[i]);
		pthread_mutex_destroy(&philosophers[i].meal_mutex);
	}
	pthread_mutex_destroy(&printf_mutex);
	free(forks);
	free(philosophers);
	return (0);
}
