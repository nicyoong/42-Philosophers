/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 23:35:00 by nyoong            #+#    #+#             */
/*   Updated: 2025/04/07 20:07:42 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	create_threads(t_philosopher *philosophers, int num_philos,
	t_init_config *config)
{
	int			i;
	pthread_t	*threads;
	pthread_t	monitor_thread;
	int			j;
	t_monitor_args monitor_args;

	threads = malloc(num_philos * sizeof(pthread_t));
	if (!threads)
		return (1);
    monitor_args.philosophers = philosophers;
    monitor_args.config = config;
	monitor_args.threads = threads;
	if (pthread_create(&monitor_thread, NULL, monitor, &monitor_args) != 0)
	{
		free(threads);
		return (1);
	}
	pthread_detach(monitor_thread);
	i = 0;
	while (i < num_philos)
	{
		if (pthread_create(&threads[i], NULL, philosopher_life, &philosophers[i]) != 0)
		{
			j = 0;
			while (j < i)
			{
				pthread_join(threads[j], NULL);
				j++;
			}
			free(threads);
			return 1;
		}
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

void	init_config_struct(t_init_config *config, pthread_mutex_t *forks,
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
	if (create_threads(philosophers, num_philos, &config))
		return (handle_thread_error(&printf_mutex, forks,
				philosophers, num_philos));
	cleanup_resources(forks, philosophers, num_philos, &printf_mutex);
	return (0);
}
