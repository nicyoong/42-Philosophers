/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philocore.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 23:35:00 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/23 19:43:23 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	create_philosopher_threads(pthread_t *threads,
	t_philosopher *philosophers, int num_philos)
{
	int	i;

	i = 0;
	while (i < num_philos)
	{
		if (pthread_create(&threads[i], NULL,
				philosopher_life, &philosophers[i]) != 0)
		{
			while (i-- > 0)
				pthread_join(threads[i], NULL);
			return (1);
		}
		precise_usleep(100);
		i++;
	}
	return (0);
}

int	create_threads(t_philosopher *philosophers, int num_philos,
	t_init_config *config, t_data *data)
{
	int				i;
	pthread_t		*threads;
	pthread_t		monitor_thread;
	t_monitor_args	monitor_args;

	threads = malloc(num_philos * sizeof(pthread_t));
	if (!threads)
		return (1);
	monitor_args.philosophers = philosophers;
	monitor_args.config = config;
	monitor_args.threads = threads;
	monitor_args.data = data;
	if (pthread_create(&monitor_thread, NULL, monitor, &monitor_args) != 0)
	{
		free(threads);
		return (1);
	}
	create_philosopher_threads(threads, philosophers, num_philos);
	pthread_join(monitor_thread, NULL);
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
		pthread_mutex_destroy(&(philosophers[i].meal_mutex));
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
	pthread_mutex_init(&config->stop_data.stop_mutex, NULL);
	config->stop_data.simulation_should_end = false;
}

void	set_printf_mutex(t_init_config *config,	pthread_mutex_t *printf_mutex)
{
	config->printf_mutex = printf_mutex;
}
