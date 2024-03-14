/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 01:52:52 by vshchuki          #+#    #+#             */
/*   Updated: 2024/03/14 12:53:18 by vshchuki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

unsigned long	gettime_usec(struct timeval time)
{
	return (time.tv_sec * 1000000 + time.tv_usec);
}

unsigned long	gettime_ms(struct timeval time)
{
	return ((time.tv_sec * 1000000 + time.tv_usec) / 1000);
}

/**
 * The condiition helps to determine if other philos are alive or current
 * philo is the first to die because check_alive returns the first philo to die
 * or 0 in case of everybody is alive.
*/
int	print_message(t_philo *philo, char *msg)
{
	struct timeval	curr_time;
	int				meal_cond;
	int				cond;
	int				err;

	meal_cond = true;
	if (philo->state->meals_per_philo > 0)
	{
		meal_cond = check_meal(philo) > 0;
	}
	cond = check_alive(philo);
	if ((cond == 0 || cond == philo->no) && meal_cond)
	{
		err = gettimeofday(&curr_time, NULL);
		if (err != EXIT_SUCCESS)
			return (EXIT_FAILURE);
		printf("%6lu %3d %s\n", gettime_ms(curr_time)
			- gettime_ms(philo->state->start_time), philo->no, msg);
	}
	return (EXIT_SUCCESS);
}

static int	death_handle(struct timeval	curr_time, t_philo *philo)
{
	if (gettime_ms(curr_time) - gettime_ms(philo->meal_time)
		> (unsigned long)philo->state->time_to_die)
	{
		pthread_mutex_lock(philo->state->death_mutex);
		if (philo->state->died_first == 0)
			philo->state->died_first = philo->no;
		pthread_mutex_unlock(philo->state->death_mutex);
		print_message(philo, "died");
		return (EXIT_PHILO_DEATH);
	}
	return (EXIT_SUCCESS);
}

int	thread_sleep(t_philo *philo, unsigned long (*f)(struct timeval),
	unsigned long time, useconds_t step)
{
	struct timeval	start;
	struct timeval	curr_time;
	unsigned long	sleeping_time;
	int				err;

	err = gettimeofday(&curr_time, NULL);
	if (err != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	start = curr_time;
	sleeping_time = f(curr_time) - f(start);
	while (sleeping_time < time)
	{
		death_handle(curr_time, philo);
		usleep(step);
		err = gettimeofday(&curr_time, NULL);
		if (err != EXIT_SUCCESS)
			return (EXIT_FAILURE);
		sleeping_time = f(curr_time) - f(start);
	}
	return (EXIT_SUCCESS);
}
