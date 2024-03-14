/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_eat.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 13:08:40 by vshchuki          #+#    #+#             */
/*   Updated: 2024/03/14 13:19:09 by vshchuki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	check_meal(t_philo *philo)
{
	int	result;

	pthread_mutex_lock(philo->state->meal_mutex);
	result = philo->state->meals_left;
	pthread_mutex_unlock(philo->state->meal_mutex);
	return (result);
}

int	handle_meals(t_philo *philo)
{
	pthread_mutex_lock(philo->state->meal_mutex);
	if (philo->state->meals_left > 0
		&& philo->meals_count != philo->state->meals_per_philo)
		philo->state->meals_left -= 1;
	else if (philo->state->meals_left == 0)
	{
		pthread_mutex_lock(philo->state->death_mutex);
		philo->state->died_first = philo->no;
		pthread_mutex_unlock(philo->state->death_mutex);
		pthread_mutex_unlock(philo->state->meal_mutex);
		return (EXIT_PHILO_DEATH);
	}
	pthread_mutex_unlock(philo->state->meal_mutex);
	philo->meals_count += 1;
	return (EXIT_SUCCESS);
}

int	eat(t_philo *philo)
{
	struct timeval	curr_time;
	int				err;

	err = gettimeofday(&curr_time, NULL);
	if (err != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	philo->meal_time = curr_time;
	print_message(philo, "is eating");
	if (philo->state->meals_per_philo > -1 && handle_meals(philo)
		== EXIT_PHILO_DEATH)
		return (EXIT_PHILO_DEATH);
	if (thread_sleep(philo, gettime_ms, philo->state->time_to_eat, 500)
		== EXIT_PHILO_DEATH)
	{
		pthread_mutex_lock(philo->state->death_mutex);
		if (philo->state->died_first == 0)
			philo->state->died_first = philo->no;
		pthread_mutex_unlock(philo->state->death_mutex);
		pthread_mutex_lock(&philo->next->fork_mutex);
		philo->next->fork_available = true;
		pthread_mutex_unlock(&philo->next->fork_mutex);
		pthread_mutex_lock(&philo->fork_mutex);
		philo->fork_available = true;
		pthread_mutex_unlock(&philo->fork_mutex);
		return (EXIT_PHILO_DEATH);
	}
	return (EXIT_SUCCESS);
}
