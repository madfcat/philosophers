/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 15:27:12 by vshchuki          #+#    #+#             */
/*   Updated: 2024/03/14 14:20:12 by vshchuki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	{
		write(2, "Wrong arguments count.\nUsage: ./philo <philos_amount> <time"
			"_to_die> <time_to_eat> <time_to_sleep> [meals_per_philo]\n", 116);
		return (EXIT_FAILURE);
	}
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
