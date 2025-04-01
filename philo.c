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

//It still eats after fulfilling all conditions that philos must eat at least n times and then immediately stop

int	validate_arguments(int argc, char **argv)
{
	int	num_philos;

	if (argc < 5 || argc > 6)
		return (printf("Error: wrong number of arguments\n"), 1);
	num_philos = ft_atoi(argv[1]);
	if (num_philos <= 0 || ft_atoi(argv[2]) <= 0 || ft_atoi(argv[3]) <= 0
		|| ft_atoi(argv[4]) <= 0 || (argc == 6 && ft_atoi(argv[5]) <= 0))
		return (printf("Error: invalid arguments\n"), 1);
	return (0);
}

int	initialize_forks(pthread_mutex_t **forks, int num_philos)
{
	int	i;

	*forks = malloc(num_philos * sizeof(pthread_mutex_t));
	if (!*forks)
		return (printf("Error: malloc failed\n"), 1);
	for (i = 0; i < num_philos; i++)
		pthread_mutex_init(&(*forks)[i], NULL);
	return (0);
}

void init_philosopher(t_philosopher *philo, int i, t_init_config *config, int argc)
{
    int required_meals = (argc == 6) ? ft_atoi(config->argv[5]) : -1;
    
    philo->id = i + 1;
    philo->left_fork = &config->forks[i];
    philo->right_fork = &config->forks[(i + 1) % config->num_philos];
    pthread_mutex_init(&philo->meal_mutex, NULL);
    philo->last_meal_time = get_current_time();
    philo->meal_count = 0;
    philo->time_to_die = ft_atoi(config->argv[2]);
    philo->time_to_eat = ft_atoi(config->argv[3]);
    philo->time_to_sleep = ft_atoi(config->argv[4]);
    philo->required_meals = required_meals;
    philo->printf_mutex = config->printf_mutex;
    philo->total_philosophers = config->num_philos;
}

int	initialize_philosophers(t_philosopher **philosophers, t_init_config *config, int argc)
{
    int i;
    
    *philosophers = malloc(config->num_philos * sizeof(t_philosopher));
    if (!*philosophers)
        return (printf("Error: malloc failed\n"), 1);
    
    for (i = 0; i < config->num_philos; i++)
        init_philosopher(&(*philosophers)[i], i, config, argc);
    
    return 0;
}

int	create_threads(t_philosopher *philosophers, int num_philos)
{
	int			i;
	pthread_t	*threads;
	pthread_t	monitor_thread;

	if (pthread_create(&monitor_thread, NULL, monitor, philosophers) != 0)
		return (1);
	pthread_detach(monitor_thread);
	threads = malloc(num_philos * sizeof(pthread_t));
	if (!threads)
		return (1);
	for (i = 0; i < num_philos; i++)
	{
		if (pthread_create(&threads[i], NULL, philosopher_life, &philosophers[i]) != 0)
			return (free(threads), 1);
		usleep(100);
	}
	for (i = 0; i < num_philos; i++)
		pthread_join(threads[i], NULL);
	free(threads);
	return (0);
}

void	cleanup_resources(pthread_mutex_t *forks, t_philosopher *philosophers,
	int num_philos, pthread_mutex_t *printf_mutex)
{
int	i;

for (i = 0; i < num_philos; i++)
{
pthread_mutex_destroy(&forks[i]);
pthread_mutex_destroy(&philosophers[i].meal_mutex);
}
pthread_mutex_destroy(printf_mutex);
free(forks);
free(philosophers);
}

int	handle_init_error(pthread_mutex_t *printf_mutex, pthread_mutex_t *forks)
{
	pthread_mutex_destroy(printf_mutex);
	free(forks);
	return (1);
}

int handle_thread_error(pthread_mutex_t *printf_mutex, 
	pthread_mutex_t *forks,
	t_philosopher *philosophers,
	int num_philos)
{
	cleanup_resources(forks, philosophers, num_philos, printf_mutex);
	printf("Error: thread creation failed\n");
	return (1);
}

int	main(int argc, char **argv)
{
	int				num_philos;
	t_philosopher	*philosophers;
	pthread_mutex_t	*forks;
	pthread_mutex_t	printf_mutex;
	t_init_config	config;

	if (validate_arguments(argc, argv))
		return (1);
	num_philos = ft_atoi(argv[1]);
	if (initialize_forks(&forks, num_philos))
		return (1);
	pthread_mutex_init(&printf_mutex, NULL);
	config = (t_init_config){
		.forks = forks,
		.num_philos = num_philos,
		.argv = argv,
		.printf_mutex = &printf_mutex
	};
	if (initialize_philosophers(&philosophers, &config, argc))
		return (handle_init_error(&printf_mutex, forks));
	if (create_threads(philosophers, num_philos))
		return (handle_thread_error(&printf_mutex, forks, philosophers, num_philos));
	cleanup_resources(forks, philosophers, num_philos, &printf_mutex);
	return (0);
}

