/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 12:53:25 by vshchuki          #+#    #+#             */
/*   Updated: 2024/03/14 13:11:28 by vshchuki         ###   ########.fr       */
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

/**
 * @param number order number of philosopher
*/
t_philo	*create_philo(int number, t_state *state)
{
	t_philo	*philo;

	philo = (t_philo *)malloc(sizeof(t_philo));
	if (!philo)
		return (NULL);
	if (pthread_mutex_init(&philo->fork_mutex, NULL) != EXIT_SUCCESS)
		return (NULL);
	if (pthread_mutex_init(&philo->status_mutex, NULL) != EXIT_SUCCESS)
		return (NULL);
	philo->no = number;
	philo->next = NULL;
	philo->prev = NULL;
	philo->fork_available = true;
	philo->meals_count = 0;
	if (gettimeofday(&philo->meal_time, NULL) != EXIT_SUCCESS)
		return (NULL);
	philo->state = state;
	return (philo);
}

void	connect_philo(t_philo *last, t_philo *new)
{
	last->next = new;
	new->prev = last;
}

int	init_philos(t_state *state)
{
	t_philo	*curr;
	int		i;

	state->head = create_philo(1, state);
	if (!state->head)
		return (EXIT_FAILURE);
	curr = state->head;
	i = 1;
	while (i < state->number_of_philosophers)
	{
		curr->next = create_philo(i + 1, state);
		if (!curr->next)
			return (EXIT_FAILURE);
		connect_philo(curr, curr->next);
		curr = curr->next;
		i++;
	}
	connect_philo(curr, state->head);
	return (EXIT_SUCCESS);
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
