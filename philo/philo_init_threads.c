/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_init_threads.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 13:24:50 by vshchuki          #+#    #+#             */
/*   Updated: 2024/03/14 13:28:04 by vshchuki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/**
 * @return NULL on success
*/
void	*routine(void *arg)
{
	t_philo			*philo;
	unsigned long	error;
	int				result;

	error = EXIT_FAILURE;
	philo = (t_philo *)arg;
	if (philo->no % 2 == 0)
	{
		usleep(10000);
	}
	if (philo->no == 1
		&& gettimeofday(&philo->state->start_time, NULL) != EXIT_SUCCESS)
		return ((void *)error);
	if (gettimeofday(&philo->meal_time, NULL) != EXIT_SUCCESS)
		return ((void *)error);
	while (check_alive(philo) == 0)
	{
		result = take_forks_eat(philo);
		if (result == EXIT_PHILO_DEATH)
			return (NULL);
	}
	return (NULL);
}

int	create_philo_threads(t_state *state)
{
	t_philo	*curr;
	int		err;

	curr = state->head;
	while (curr->next != state->head)
	{
		err = pthread_create(&curr->id, NULL, &routine, curr);
		if (err != 0)
			return (err);
		curr = curr->next;
	}
	err = pthread_create(&curr->id, NULL, &routine, curr);
	if (err != 0)
		return (err);
	return (EXIT_SUCCESS);
}

int	join_philo_threads(t_state *state)
{
	t_philo	*curr;
	int		err;

	curr = state->head;
	while (curr->next != state->head)
	{
		err = pthread_join(curr->id, NULL);
		if (err != 0)
			return (err);
		curr = curr->next;
	}
	err = pthread_join(curr->id, NULL);
	if (err != 0)
		return (err);
	return (EXIT_SUCCESS);
}
