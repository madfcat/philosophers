/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 15:27:12 by vshchuki          #+#    #+#             */
/*   Updated: 2024/03/14 13:08:29 by vshchuki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


/*
Arguments:
number_of_philosophers time_to_die time_to_eat
time_to_sleep
[meals_per_philo]
*/


/*
Allowed funcitons:
memset, printf, malloc, free, write,
usleep, gettimeofday, pthread_create,
pthread_detach, pthread_join, pthread_mutex_init,
pthread_mutex_destroy, pthread_mutex_lock,
pthread_mutex_unlock
*/

/* int	number_of_philosophers = 17;
int	time_to_die = 200;
int	time_to_eat = 70;
int	time_to_sleep = 50;
int meals_per_philo = 10; */

#include "philo.h"

/**
 * @return 0 if noone died, otherwise, should return philo number
*/
int	check_alive(t_philo *philo)
{
	int	result;

	pthread_mutex_lock(philo->state->death_mutex);
	result = philo->state->died_first;
	pthread_mutex_unlock(philo->state->death_mutex);
	return (result);
}

t_bool	check_fork(t_philo *philo)
{
	int	result;

	pthread_mutex_lock(&philo->fork_mutex);
	result = philo->fork_available;
	pthread_mutex_unlock(&philo->fork_mutex);
	return (result);
}

int	main(const int argc, char const *argv[])
{
	pthread_mutex_t	state_mutex;
	pthread_mutex_t	meal_mutex;
	t_state			state;

	if (argc < 5 || argc > 6)
		return (EXIT_FAILURE);
	if (init_state(&state, argv, &state_mutex, &meal_mutex) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (init_philos(&state) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (create_philo_threads(&state) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (join_philo_threads(&state) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (pthread_mutex_destroy(&state_mutex) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (pthread_mutex_destroy(&meal_mutex) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (free_philos(state.head) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}
