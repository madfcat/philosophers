/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 01:52:52 by vshchuki          #+#    #+#             */
/*   Updated: 2024/03/12 20:16:40 by vshchuki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"


unsigned long	gettime_usec(struct timeval time)
{
	return (time.tv_sec * 1000000 + time.tv_usec);
}

unsigned long	gettime_ms(struct timeval time)
{
	return (time.tv_sec * 1000000 + time.tv_usec) / 1000;
}

int	print_message(t_philo *philo, char *msg)
{
	struct timeval	curr_time;
	int 			err;

	if (check_alive(philo) || philo->died_first)
	{
		err = gettimeofday(&curr_time, NULL);
		if (err != EXIT_SUCCESS)
			return (EXIT_FAILURE);
		printf("%6lu %2d %s\n",
			gettime_ms(curr_time) - gettime_ms(philo->state->start_time), philo->no, msg);
	}
	return (EXIT_SUCCESS);
}

int	free_philos(t_philo *head)
{
	t_philo *prev;
	t_philo *curr;

	curr = head;
	while (curr)
	{
		if (pthread_mutex_destroy(&curr->fork_mutex) != EXIT_SUCCESS)
			return (EXIT_FAILURE);
		if (pthread_mutex_destroy(&curr->status_mutex) != EXIT_SUCCESS)
			return (EXIT_FAILURE);
		// if (pthread_mutex_destroy(&curr->fork_available_mutex) != EXIT_SUCCESS)
		// 	return (EXIT_FAILURE);
		prev = curr;
		curr = curr->next;
		free(prev);
		if (curr == head)
			break;
	}
	return (EXIT_SUCCESS);
}

int	thread_sleep(t_philo *philo, int ms)
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
	start = curr_time;
	sleeping_time = gettime_ms(curr_time) - gettime_ms(start);
	while (sleeping_time < (unsigned long)ms)
	{
		// if (philo->no == 1)
			// printf("philo %d has no meal for: %lu\n", philo->no, gettime_ms(curr_time) - gettime_ms(philo->meal_time));
		if (gettime_ms(curr_time) - gettime_ms(philo->meal_time) > (unsigned long)philo->state->time_to_die)
		{
			// if (philo->state->still_alive)
			if (check_alive(philo))
			{
				print_message(philo, "died");
				pthread_mutex_lock(philo->state->death_mutex);
					philo->state->still_alive = false;
				pthread_mutex_unlock(philo->state->death_mutex);	
				// philo->state->alive[philo->no - 1] = false;
			}
			return (EXIT_PHILO_DEATH);
		}
		usleep(500);
		err = gettimeofday(&curr_time, NULL);
		if (err != EXIT_SUCCESS)
			return (EXIT_FAILURE);
		sleeping_time = gettime_ms(curr_time) - gettime_ms(start);
	}
	return (EXIT_SUCCESS);
}

int	thread_sleep_usec(t_philo *philo, int usec)
{
	struct timeval	start;
	struct timeval	curr_time;
	unsigned long	sleeping_time;
	int err;
	
	// start = philo->state->curr_time;
	err = gettimeofday(&start, NULL);
		if (err != EXIT_SUCCESS)
			return (1);
	err = gettimeofday(&curr_time, NULL);
		if (err != EXIT_SUCCESS)
			return (1);
	sleeping_time = gettime_usec(curr_time) - gettime_usec(start);
	while (sleeping_time < (unsigned long)usec)
	{
			// printf("philo %d has no meal for: %lu\n", philo->no, gettime_ms(curr_time) - gettime_ms(philo->meal_time));
	
		if (gettime_ms(curr_time) - gettime_ms(philo->meal_time) > (unsigned long)philo->state->time_to_die)
		{
			if (check_alive(philo))
			{
				print_message(philo, "died");
				pthread_mutex_lock(philo->state->death_mutex);
				philo->state->still_alive = false;
				pthread_mutex_unlock(philo->state->death_mutex);
				
/* 				pthread_mutex_lock(philo->state->death_mutex);
				printf("philo %d still_alive: %d\n", philo->no, philo->state->still_alive);
				pthread_mutex_unlock(philo->state->death_mutex); */
				// philo->state->alive[philo->no - 1] = false;
			}

			return (EXIT_PHILO_DEATH);
		}
		usleep(100);
		err = gettimeofday(&curr_time, NULL);
		if (err != EXIT_SUCCESS)
			return (EXIT_FAILURE);
		sleeping_time = gettime_usec(curr_time) - gettime_usec(start);
	}
	return (EXIT_SUCCESS);
}