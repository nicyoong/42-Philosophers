/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 23:35:00 by nyoong            #+#    #+#             */
/*   Updated: 2025/03/31 19:36:18 by nyoong           ###   ########.fr       */
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
		think(philo);
		take_forks(philo);
		update_last_meal(philo);
		eat(philo);
		release_forks(philo);
		update_meal_count(philo);
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
	i = -1;
	while (++i < num_philos)
	{
		pthread_mutex_lock(&philos[i].meal_mutex);
		if (philos[i].meal_count < required)
			all_ate = false;
		pthread_mutex_unlock(&philos[i].meal_mutex);
		if (!all_ate)
			break ;
	}
	return all_ate;
}

void		*monitor(void *arg)
{
	t_philosopher	*philos;
	int				num_philos;
	int				req_meals;
	unsigned long	current_time;
	int				i;

	philos = (t_philosopher *)arg;
	num_philos = philos[0].total_philosophers;
	req_meals = philos[0].required_meals;
	while (true)
	{
		current_time = get_current_time();
		i = -1;
		while (++i < num_philos)
		{
			if (check_philosopher_status(&philos[i], current_time))
				handle_philosopher_death(&philos[i]);
		}
		if (req_meals != -1 && check_meal_completion(philos, num_philos, req_meals))
			exit(EXIT_SUCCESS);
		usleep(1000);
	}
	return (NULL);
}

int	validate_arguments(int argc, char **argv)
{
	if (argc < 5 || argc > 6)
	{
		printf("Usage: %s number_of_philosophers time_to_die ", argv[0]);
		printf("time_to_eat time_to_sleep [required_meals]\n");
		return (1);
	}
	return (0);
}

int	parse_arguments(t_simulation *sim, int argc, char **argv)
{
	sim->num_philos = ft_atoi(argv[1]);
	sim->time_to_die = ft_atoi(argv[2]);
	sim->time_to_eat = ft_atoi(argv[3]);
	sim->time_to_sleep = ft_atoi(argv[4]);
	sim->required_meals = (argc == 6) ? ft_atoi(argv[5]) : -1;

	if (sim->num_philos <= 0 || sim->time_to_die <= 0 
		|| sim->time_to_eat <= 0 || sim->time_to_sleep <= 0 
		|| (argc == 6 && sim->required_meals <= 0))
	{
		printf("Invalid arguments\n");
		return (1);
	}
	return (0);
}

int	initialize_resources(t_simulation *sim)
{
	sim->forks = malloc(sim->num_philos * sizeof(pthread_mutex_t));
	sim->philos = malloc(sim->num_philos * sizeof(t_philosopher));
	
	if (!sim->forks || !sim->philos)
		return (1);
	
	pthread_mutex_init(&sim->printf_mutex, NULL);
	for (int i = 0; i < sim->num_philos; i++)
	{
		if (pthread_mutex_init(&sim->forks[i], NULL) != 0)
			return (1);
	}
	return (0);
}

void	setup_philosophers(t_simulation *sim)
{
	for (int i = 0; i < sim->num_philos; i++)
	{
		sim->philos[i] = (t_philosopher){
			.id = i + 1,
			.left_fork = &sim->forks[i],
			.right_fork = &sim->forks[(i + 1) % sim->num_philos],
			.last_meal_time = get_current_time(),
			.meal_count = 0,
			.time_to_die = sim->time_to_die,
			.time_to_eat = sim->time_to_eat,
			.time_to_sleep = sim->time_to_sleep,
			.required_meals = sim->required_meals,
			.printf_mutex = &sim->printf_mutex,
			.total_philosophers = sim->num_philos
		};
		pthread_mutex_init(&sim->philos[i].meal_mutex, NULL);
	}
}

int	start_simulation(t_simulation *sim)
{
	pthread_t	monitor_thread;

	for (int i = 0; i < sim->num_philos; i++)
	{
		if (i % 2 == 0)
			usleep(1000);
		if (pthread_create(&sim->threads[i], NULL, 
			philosopher_life, &sim->philos[i]) != 0)
			return (1);
	}
	if (pthread_create(&monitor_thread, NULL, monitor, sim->philos) != 0)
		return (1);
	pthread_join(monitor_thread, NULL);
	return (0);
}

void	cleanup_resources(t_simulation *sim)
{
	for (int i = 0; i < sim->num_philos; i++)
	{
		pthread_mutex_destroy(&sim->forks[i]);
		pthread_mutex_destroy(&sim->philos[i].meal_mutex);
	}
	pthread_mutex_destroy(&sim->printf_mutex);
	free(sim->forks);
	free(sim->philos);
}


// int main(int argc, char **argv) {
//     if (argc < 5 || argc > 6) {
//         printf("Usage: %s number_of_philosophers time_to_die time_to_eat time_to_sleep [required_meals]\n", argv[0]);
//         return 1;
//     }
//     int num_philos = atoi(argv[1]);
//     int time_to_die = atoi(argv[2]);
//     int time_to_eat = atoi(argv[3]);
//     int time_to_sleep = atoi(argv[4]);
//     int required_meals = argc == 6 ? atoi(argv[5]) : -1;
//     if (num_philos <= 0 || time_to_die <= 0 || time_to_eat <= 0 || time_to_sleep <= 0 || (argc == 6 && required_meals <= 0)) {
//         printf("Invalid arguments\n");
//         return 1;
//     }
//     pthread_mutex_t *forks = malloc(num_philos * sizeof(pthread_mutex_t));
//     for (int i = 0; i < num_philos; i++) {
//         pthread_mutex_init(&forks[i], NULL);
//     }
//     pthread_mutex_t printf_mutex;
//     pthread_mutex_init(&printf_mutex, NULL);
//     t_philosopher *philosophers = malloc(num_philos * sizeof(t_philosopher));
//     for (int i = 0; i < num_philos; i++) {
//         philosophers[i].id = i + 1;
//         philosophers[i].left_fork = &forks[i];
//         philosophers[i].right_fork = &forks[(i + 1) % num_philos];
//         pthread_mutex_init(&philosophers[i].meal_mutex, NULL);
//         philosophers[i].last_meal_time = get_current_time();
//         philosophers[i].meal_count = 0;
//         philosophers[i].time_to_die = time_to_die;
//         philosophers[i].time_to_eat = time_to_eat;
//         philosophers[i].time_to_sleep = time_to_sleep;
//         philosophers[i].required_meals = required_meals;
//         philosophers[i].printf_mutex = &printf_mutex;
//         philosophers[i].total_philosophers = num_philos;
//     }
//     pthread_t threads[num_philos];
//     for (int i = 0; i < num_philos; i++) {
// 		if (i % 2 == 0) usleep(1000);
//         pthread_create(&threads[i], NULL, philosopher_life, &philosophers[i]);
//     }
//     pthread_t monitor_thread;
//     pthread_create(&monitor_thread, NULL, monitor, philosophers);
//     pthread_join(monitor_thread, NULL);
//     for (int i = 0; i < num_philos; i++) {
//         pthread_mutex_destroy(&forks[i]);
//         pthread_mutex_destroy(&philosophers[i].meal_mutex);
//     }
//     pthread_mutex_destroy(&printf_mutex);
//     free(forks);
//     free(philosophers);
//     return 0;
// }
