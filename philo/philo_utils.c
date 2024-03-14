/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 01:52:52 by vshchuki          #+#    #+#             */
/*   Updated: 2024/03/14 03:24:01 by vshchuki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	check_arg(const char *str)
{
	char	*converted;
	int		result;

	converted = ft_itoa(ft_atoi(str));
	result = ft_strncmp(str, converted, ft_strlen(str) + 1);
	free(converted);
	return (result);
}

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
		printf("%6lu %2d %s\n", gettime_ms(curr_time)
			- gettime_ms(philo->state->start_time), philo->no, msg);
	}
	return (EXIT_SUCCESS);
}

int	free_philos(t_philo *head)
{
	t_philo	*prev;
	t_philo	*curr;

	curr = head;
	while (curr)
	{
		if (pthread_mutex_destroy(&curr->fork_mutex) != EXIT_SUCCESS)
			return (EXIT_FAILURE);
		if (pthread_mutex_destroy(&curr->status_mutex) != EXIT_SUCCESS)
			return (EXIT_FAILURE);
		prev = curr;
		curr = curr->next;
		free(prev);
		if (curr == head)
			break ;
	}
	return (EXIT_SUCCESS);
}

int	thread_sleep(t_philo *philo, unsigned long (*f)(struct timeval),
	unsigned long time, useconds_t step)
{
	struct timeval	start;
	struct timeval	curr_time;
	unsigned long	sleeping_time;
	int err;

	err = gettimeofday(&curr_time, NULL);
	if (err != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	start = curr_time;
	sleeping_time = f(curr_time) - f(start);
	while (sleeping_time < time)
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
		usleep(step);
		err = gettimeofday(&curr_time, NULL);
		if (err != EXIT_SUCCESS)
			return (EXIT_FAILURE);
		sleeping_time = f(curr_time) - f(start);
	}
	return (EXIT_SUCCESS);
}

/* int	thread_sleep_usec(t_philo *philo, int usec)
{
	struct timeval	start;
	struct timeval	curr_time;
	unsigned long	sleeping_time;
	int err;
	
	err = gettimeofday(&start, NULL);
		if (err != EXIT_SUCCESS)
			return (1);
	err = gettimeofday(&curr_time, NULL);
		if (err != EXIT_SUCCESS)
			return (1);
	sleeping_time = gettime_usec(curr_time) - gettime_usec(start);
	while (sleeping_time < (unsigned long)usec)
	{
		if (gettime_ms(curr_time) - gettime_ms(philo->meal_time) > (unsigned long)philo->state->time_to_die)
		{
				pthread_mutex_lock(philo->state->death_mutex);
				if (philo->state->died_first == 0)
					philo->state->died_first = philo->no;
				pthread_mutex_unlock(philo->state->death_mutex);
				print_message(philo, "died");
			return (EXIT_PHILO_DEATH);
		}
		usleep(100);
		err = gettimeofday(&curr_time, NULL);
		if (err != EXIT_SUCCESS)
			return (EXIT_FAILURE);
		sleeping_time = gettime_usec(curr_time) - gettime_usec(start);
	}
	return (EXIT_SUCCESS);
} */