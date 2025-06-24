/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philolife2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 00:04:59 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/24 00:37:19 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	update_meal_count(t_philosopher *philo)
{
	pthread_mutex_lock(&philo->meal_mutex);
	philo->meal_count++;
	pthread_mutex_unlock(&philo->meal_mutex);
}

void	philo_sleep(t_philosopher *philo)
{
	print_message(philo, "is sleeping");
	precise_usleep(philo->time_to_sleep);
}

static void handle_single_philo(t_philosopher *philo)
{
    print_message(philo, "is thinking");
    pthread_mutex_lock(philo->left_fork);
    print_message(philo, "has taken a fork");
    precise_usleep(philo->time_to_die);
}

static void initial_delay_if_even(t_philosopher *philo)
{
    if (philo->id % 2 == 0)
        precise_usleep(100);
}

// Returns 1 if the loop should break, 0 otherwise
static int perform_cycle(t_philosopher *philo, t_data *stop_data)
{
    if (should_stop(stop_data))
        return 1;
    think(philo);
    if (should_stop(stop_data))
        return 1;
    take_forks(philo);
    if (should_stop(stop_data))
    {
        release_forks(philo);
        return 1;
    }
    update_last_meal(philo);
    if (should_stop(stop_data))
    {
        release_forks(philo);
        return 1;
    }
    eat(philo);
    update_meal_count(philo);
    release_forks(philo);
    if (should_stop(stop_data))
        return 1;
    philo_sleep(philo);
    return 0;
}

static void philosopher_loop(t_philosopher *philo)
{
    t_data *stop_data = &philo->config->stop_data;

    while (!perform_cycle(philo, stop_data))
        ;
}

void *philosopher_life(void *arg)
{
    t_philosopher *philo = (t_philosopher *)arg;

    if (philo->config->num_philos == 1)
    {
        handle_single_philo(philo);
        return NULL;
    }

    initial_delay_if_even(philo);
    philosopher_loop(philo);

    return NULL;
}
