/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 23:35:00 by nyoong            #+#    #+#             */
/*   Updated: 2025/04/03 00:06:06 by nyoong           ###   ########.fr       */
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

bool check_philosopher_status(t_philosopher *philo)
{
	bool			starving;
	unsigned long	time_since_meal;
	unsigned long	current_time;

	current_time = get_current_time();
	pthread_mutex_lock(&philo->meal_mutex);
	time_since_meal = current_time - philo->last_meal_time;
	starving = (time_since_meal > philo->time_to_die);
	pthread_mutex_unlock(&philo->meal_mutex);
	return (starving);
}

bool	check_meal_completion(t_philosopher *philos,
	int num_philos, int required)
{
	int	i;

	i = -1;
	while (++i < num_philos) {
		pthread_mutex_lock(&philos[i].meal_mutex);
		if (philos[i].meal_count < required) {
			pthread_mutex_unlock(&philos[i].meal_mutex);
			return (false);
		}
		pthread_mutex_unlock(&philos[i].meal_mutex);
	}
	return (true);
}

void		*monitor(void *arg)
{
	t_philosopher	*philos;
	int				num_philos;
	int				req_meals;
	int				i;

	philos = (t_philosopher *)arg;
	num_philos = philos[0].total_philosophers;
	req_meals = philos[0].required_meals;
	while (true)
	{
		i = -1;
		while (++i < num_philos)
		{
			if (check_philosopher_status(&philos[i]))
				handle_philosopher_death(&philos[i]);
		}
		if (req_meals != -1
			&& check_meal_completion(philos, num_philos, req_meals))
			exit(EXIT_SUCCESS);
		precise_usleep(100);
	}
	return (NULL);
}

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
	i = 0;
	while (i < num_philos)
	{
		pthread_mutex_init(&(*forks)[i], NULL);
		i++;
	}
	return (0);
}

void init_philosopher(t_philosopher *philo,
	int i, t_init_config *config, int argc)
{
	int required_meals = (argc == 6) ? ft_atoi(config->argv[5]) : -1;

	philo->id = i + 1;
	philo->left_fork = &config->forks[i];
	philo->right_fork = &config->forks[(i + 1) % config->num_philos];
	pthread_mutex_init(&philo->meal_mutex, NULL);
	philo->last_meal_time = get_current_time();
	philo->meal_count = 0;
	philo->time_to_die = ft_atoi(config->argv[2]) * 1000;
	philo->time_to_eat = ft_atoi(config->argv[3]) * 1000;
	philo->time_to_sleep = ft_atoi(config->argv[4]) * 1000;
	philo->required_meals = required_meals;
	philo->printf_mutex = config->printf_mutex;
	philo->total_philosophers = config->num_philos;
}

int	initialize_philosophers(t_philosopher **philosophers,
	t_init_config *config, int argc)
{
	int i;

	*philosophers = malloc(config->num_philos * sizeof(t_philosopher));
	if (!*philosophers)
		return (printf("Error: malloc failed\n"), 1);
	i = 0;
	while (i < config->num_philos)
	{
		init_philosopher(&(*philosophers)[i], i, config, argc);
		i++;
	}
	return (0);
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
	i = 0;
	while (i < num_philos)
	{
		if (pthread_create(&threads[i], NULL, philosopher_life, &philosophers[i]) != 0)
			return (free(threads), 1);
		precise_usleep(100);
		i++;
	}
	i = 0;
	while (i < num_philos)
		pthread_join(threads[i++], NULL);
	free(threads);
	return (0);
}

void	cleanup_resources(pthread_mutex_t *forks, t_philosopher *philosophers,
	int num_philos, pthread_mutex_t *printf_mutex)
{
	int	i;

	i = 0;
	while (i < num_philos)
	{
		pthread_mutex_destroy(&forks[i]);
		pthread_mutex_destroy(&philosophers[i].meal_mutex);
		i++;
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
	pthread_mutex_t *forks,	t_philosopher *philosophers, int num_philos)
{
	cleanup_resources(forks, philosophers, num_philos, printf_mutex);
	printf("Error: thread creation failed\n");
	return (1);
}

void init_config_struct(t_init_config *config, 	pthread_mutex_t *forks,
	int num_philos,	char **argv)
{
	*config = (t_init_config){
	.forks = forks,
	.num_philos = num_philos,
	.argv = argv,
	.printf_mutex = NULL
	};
}

void	set_printf_mutex(t_init_config *config,	pthread_mutex_t *printf_mutex)
{
	config->printf_mutex = printf_mutex;
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
	init_config_struct(&config, forks, num_philos, argv);
	set_printf_mutex(&config, &printf_mutex);
	if (initialize_philosophers(&philosophers, &config, argc))
		return (handle_init_error(&printf_mutex, forks));
	if (create_threads(philosophers, num_philos))
		return (handle_thread_error(&printf_mutex, forks,
			philosophers, num_philos));
	cleanup_resources(forks, philosophers, num_philos, &printf_mutex);
	return (0);
}

