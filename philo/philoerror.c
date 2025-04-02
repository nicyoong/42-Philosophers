/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philoerror.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 00:07:17 by nyoong            #+#    #+#             */
/*   Updated: 2025/04/03 00:07:56 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	handle_init_error(pthread_mutex_t *printf_mutex, pthread_mutex_t *forks)
{
	pthread_mutex_destroy(printf_mutex);
	free(forks);
	return (1);
}
