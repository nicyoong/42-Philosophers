/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 00:40:00 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/23 19:08:30 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	main(int argc, char **argv)
{
	int				num_philos;
	t_philosopher	*philosophers;
	pthread_mutex_t	*forks;
	pthread_mutex_t	printf_mutex;
	t_init_config	config;
	t_data			data;

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
	pthread_mutex_init(&data.stop_mutex, NULL);
	data.simulation_should_end = false;
	if (create_threads(philosophers, num_philos, &config, &data))
		return (handle_thread_error(&printf_mutex, forks,
				philosophers, num_philos));
	cleanup_resources(forks, philosophers, num_philos, &printf_mutex);
	return (0);
}
