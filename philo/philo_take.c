/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_take.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 13:04:18 by vshchuki          #+#    #+#             */
/*   Updated: 2024/03/14 13:19:54 by vshchuki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	check_fork_take(t_philo *philo)
{
	int	result;

	pthread_mutex_lock(&philo->fork_mutex);
	result = philo->fork_available;
	if (result == true)
	{
		philo->fork_available = false;
		pthread_mutex_unlock(&philo->fork_mutex);
		return (2);
	}
	pthread_mutex_unlock(&philo->fork_mutex);
	return (result);
}

void	fork_back(t_philo *philo)
{
	pthread_mutex_lock(&philo->fork_mutex);
	if (philo->fork_available == false)
		philo->fork_available = true;
	pthread_mutex_unlock(&philo->fork_mutex);
}

int	first_fork(t_philo *philo)
{
	pthread_mutex_lock(&philo->fork_mutex);
	if (philo->fork_available)
	{
		philo->fork_available = false;
	}
	else
	{
		pthread_mutex_unlock(&philo->fork_mutex);
		return (EXIT_FAILURE);
	}
	pthread_mutex_unlock(&philo->fork_mutex);
	print_message(philo, "has taken the first fork");
	return (EXIT_SUCCESS);
}

int	second_fork(t_philo *philo)
{
	if (philo->no != philo->next->no)
	{
		while (check_fork_take(philo->next) != 2)
		{
			if (thread_sleep(philo, gettime_usec, 1, 150) == EXIT_PHILO_DEATH)
				return (EXIT_PHILO_DEATH);
		}
		print_message(philo, "has taken the second fork");
		return (EXIT_SUCCESS);
	}
	return (EXIT_FAILURE);
}

int	take_forks_eat(t_philo *philo)
{
	int second_res;

	while (check_fork_take(philo) != 2)
	{
		if (thread_sleep(philo, gettime_usec, 1, 150) == EXIT_PHILO_DEATH)
			return (EXIT_PHILO_DEATH);
	}
	print_message(philo, "has taken the first fork");

	second_res = second_fork(philo);
	if (second_res != EXIT_SUCCESS)
	{
		return (second_res);
	}
	if (eat(philo) == EXIT_PHILO_DEATH)
		return (EXIT_PHILO_DEATH);
	fork_back(philo);
	fork_back(philo->next);
	if (thread_sleep(philo, gettime_usec, 1, 1) == EXIT_PHILO_DEATH)
		return (EXIT_PHILO_DEATH);
	print_message(philo, "is sleeping");
	if (thread_sleep(philo, gettime_ms, philo->state->time_to_sleep, 500)
		== EXIT_PHILO_DEATH)
		return (EXIT_PHILO_DEATH);
	if (thread_sleep(philo, gettime_usec, 1, 1) == EXIT_PHILO_DEATH)
		return (EXIT_PHILO_DEATH);
	print_message(philo, "is thinking");
	return (EXIT_SUCCESS);
}
