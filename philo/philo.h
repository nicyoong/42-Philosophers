/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 23:30:52 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/22 00:37:47 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <unistd.h>
# include <pthread.h>
# include <sys/time.h>

typedef struct s_philosopher
{
	int				id;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	meal_mutex;
	unsigned long	last_meal_time;
	int				meal_count;
	unsigned long	time_to_die;
	unsigned long	time_to_eat;
	unsigned long	time_to_sleep;
	int				required_meals;
	pthread_mutex_t	*printf_mutex;
	int				total_philosophers;
}	t_philosopher;

typedef struct s_data
{
	pthread_mutex_t	stop_mutex;
	bool			simulation_should_end;
}	t_data;

typedef struct s_init_config
{
	pthread_mutex_t	*forks;
	int				num_philos;
	char			**argv;
	pthread_mutex_t	*printf_mutex;
}	t_init_config;

typedef struct s_monitor_args
{
	t_philosopher	*philosophers;
	t_init_config	*config;
	pthread_t		*threads;
}	t_monitor_args;

int				ft_atoi(const char *str);
unsigned long	get_current_time(void);
void			precise_usleep(unsigned long usec);
void			print_message(t_philosopher *philo, const char *msg);
void			think(t_philosopher *philo);
void			take_forks(t_philosopher *philo);
void			update_last_meal(t_philosopher *philo);
void			eat(t_philosopher *philo);
void			release_forks(t_philosopher *philo);
void			update_meal_count(t_philosopher *philo);
void			philo_sleep(t_philosopher *philo);
void			*philosopher_life(void *arg);
void			handle_philosopher_death(t_philosopher *philo,
					t_init_config *config, pthread_t *threads);
bool			check_philosopher_status(t_philosopher *philo);
bool			check_meal_completion(t_philosopher *philos,
					int num_philos, int required);
void			*monitor(void *arg);
int				validate_arguments(int argc, char **argv);
int				initialize_forks(pthread_mutex_t **forks, int num_philos);
void			init_philosopher(t_philosopher *philo,
					int i, t_init_config *config, int argc);
int				initialize_philosophers(t_philosopher **philosophers,
					t_init_config *config, int argc);
int				create_threads(t_philosopher *philosophers, int num_philos,
					t_init_config *config);
void			cleanup_resources(pthread_mutex_t *forks,
					t_philosopher *philosophers,
					int num_philos, pthread_mutex_t *printf_mutex);
int				handle_init_error(pthread_mutex_t *printf_mutex,
					pthread_mutex_t *forks);
int				handle_thread_error(pthread_mutex_t *printf_mutex,
					pthread_mutex_t *forks,	t_philosopher *philosophers,
					int num_philos);
void			init_config_struct(t_init_config *config,
					pthread_mutex_t *forks,
					int num_philos,	char **argv);
void			set_printf_mutex(t_init_config *config,
					pthread_mutex_t *printf_mutex);

#endif