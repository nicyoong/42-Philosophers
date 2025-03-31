/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 23:35:00 by nyoong            #+#    #+#             */
/*   Updated: 2025/03/31 19:17:04 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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


// void *monitor(void *arg) {
// 	t_philosopher *philosophers = (t_philosopher *)arg;
// 	int required_meals = philosophers[0].required_meals;
// 	int num_philos = philosophers[0].total_philosophers;
// 	while (1) {
// 		unsigned long current_time = get_current_time();
// 		int all_ate_enough = 1;
// 		for (int i = 0; i < num_philos; i++) {
// 			pthread_mutex_lock(&philosophers[i].meal_mutex);
// 			unsigned long last_meal = philosophers[i].last_meal_time;
// 			int meals = philosophers[i].meal_count;
// 			pthread_mutex_unlock(&philosophers[i].meal_mutex);
// 			if (current_time - last_meal >= philosophers[i].time_to_die) {
// 				pthread_mutex_lock(philosophers[i].printf_mutex);
// 				printf("%lu %d died\n", current_time, philosophers[i].id);
// 				exit(0);
// 			}
// 			if (required_meals != -1 && meals < required_meals) {
// 				all_ate_enough = 0;
// 			}
// 		}
// 		if (required_meals != -1 && all_ate_enough) {
// 			exit(0);
// 		}
// 		usleep(1000);
// 	}
// 	return NULL;
// }

int main(int argc, char **argv) {
    if (argc < 5 || argc > 6) {
        printf("Usage: %s number_of_philosophers time_to_die time_to_eat time_to_sleep [required_meals]\n", argv[0]);
        return 1;
    }
    int num_philos = atoi(argv[1]);
    int time_to_die = atoi(argv[2]);
    int time_to_eat = atoi(argv[3]);
    int time_to_sleep = atoi(argv[4]);
    int required_meals = argc == 6 ? atoi(argv[5]) : -1;
    if (num_philos <= 0 || time_to_die <= 0 || time_to_eat <= 0 || time_to_sleep <= 0 || (argc == 6 && required_meals <= 0)) {
        printf("Invalid arguments\n");
        return 1;
    }
    pthread_mutex_t *forks = malloc(num_philos * sizeof(pthread_mutex_t));
    for (int i = 0; i < num_philos; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }
    pthread_mutex_t printf_mutex;
    pthread_mutex_init(&printf_mutex, NULL);
    t_philosopher *philosophers = malloc(num_philos * sizeof(t_philosopher));
    for (int i = 0; i < num_philos; i++) {
        philosophers[i].id = i + 1;
        philosophers[i].left_fork = &forks[i];
        philosophers[i].right_fork = &forks[(i + 1) % num_philos];
        pthread_mutex_init(&philosophers[i].meal_mutex, NULL);
        philosophers[i].last_meal_time = get_current_time();
        philosophers[i].meal_count = 0;
        philosophers[i].time_to_die = time_to_die;
        philosophers[i].time_to_eat = time_to_eat;
        philosophers[i].time_to_sleep = time_to_sleep;
        philosophers[i].required_meals = required_meals;
        philosophers[i].printf_mutex = &printf_mutex;
        philosophers[i].total_philosophers = num_philos;
    }
    pthread_t threads[num_philos];
    for (int i = 0; i < num_philos; i++) {
		if (i % 2 == 0) usleep(1000);
        pthread_create(&threads[i], NULL, philosopher_life, &philosophers[i]);
    }
    pthread_t monitor_thread;
    pthread_create(&monitor_thread, NULL, monitor, philosophers);
    pthread_join(monitor_thread, NULL);
    for (int i = 0; i < num_philos; i++) {
        pthread_mutex_destroy(&forks[i]);
        pthread_mutex_destroy(&philosophers[i].meal_mutex);
    }
    pthread_mutex_destroy(&printf_mutex);
    free(forks);
    free(philosophers);
    return 0;
}
