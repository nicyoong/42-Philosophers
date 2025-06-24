/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 00:40:00 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/23 23:58:46 by nyoong           ###   ########.fr       */
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
	if (create_threads(philosophers, num_philos, &config, &config.stop_data))
		return (handle_thread_error(&printf_mutex, forks,
				philosophers, num_philos));
	cleanup_resources(forks, philosophers, num_philos, &printf_mutex);
	return (0);
}
