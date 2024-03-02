/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 01:52:52 by vshchuki          #+#    #+#             */
/*   Updated: 2024/03/02 19:43:52 by vshchuki         ###   ########.fr       */
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
	if (philo->state->still_alive)
		printf("%6lu %2d %s\n",
			gettime_ms(philo->state->curr_time) - gettime_ms(philo->state->start_time), philo->no, msg);
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
	unsigned long	sleeping_time;
	
	start = philo->state->curr_time;
	// while (gettime_usec(philo->state->curr_time) - (gettime_usec(start)) < (unsigned long)ms * 1000)
	sleeping_time = gettime_ms(philo->state->curr_time) - gettime_ms(start);
	while (sleeping_time < (unsigned long)ms)
	// while (gettime_ms(philo->state->curr_time) - (gettime_ms(start)) < (unsigned long)philo->state->time_to_sleep)
	{
		if (!philo->state->still_alive)
		{
			return (EXIT_OTHER_DEATH);
		}
		if (gettime_ms(philo->state->curr_time) - gettime_ms(philo->meal_time) > (unsigned long)philo->state->time_to_die)
		{
			if (philo->state->still_alive)
				print_message(philo, "died");
			philo->state->still_alive = false;
			return (EXIT_PHILO_DEATH);
		}
		usleep(5);
		sleeping_time = gettime_ms(philo->state->curr_time) - gettime_ms(start);
	}
	return (0);
}