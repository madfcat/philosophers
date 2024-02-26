/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 15:27:12 by vshchuki          #+#    #+#             */
/*   Updated: 2024/02/26 23:05:50 by vshchuki         ###   ########.fr       */
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
 * @return NULL on success
*/
void *routine(void	*arg)
{
	t_philo	*philo;
	unsigned long error;

	error = EXIT_FAILURE;
	philo = (t_philo *)arg;
	if (gettimeofday(&philo->meal_time, NULL) != EXIT_SUCCESS)
		return (void *)error;

	while(philo->state->still_alive)
	{
		if (philo->state->still_alive && !philo->is_thinking)
		{
			print_message(philo, "is thinking");
			philo->is_thinking = true;
		}
		// take forks
		if (philo->state->still_alive && philo->fork_available && philo->next->fork_available)
		{
			pthread_mutex_lock(philo->mutex);
			philo->fork_available = false;
			print_message(philo, "has taken a fork");
			philo->next->fork_available = false;
			print_message(philo, "has taken a fork");
			pthread_mutex_unlock(philo->mutex);
			philo->forks_in_use += 2;
		}

		// eat 
		// printf("philo %lu is eating!\n", (unsigned long)philo->id);
		if (philo->state->still_alive && philo->forks_in_use == 2) {
			gettimeofday(&philo->meal_time, NULL);
			// printf("philo %d is eating!\n", philo->no);
			philo->is_thinking = false;
			print_message(philo, "is eating");
			if (philo->state->meals_per_philo > 0)
			{
				pthread_mutex_lock(philo->state->meal_mutex);
				philo->state->meals_left -= 1;
				pthread_mutex_unlock(philo->state->meal_mutex);
			}
			usleep(philo->state->time_to_eat * 1000);
			if (!philo->state->meals_left)
			{
				philo->state->still_alive = false;
				return (NULL);
			}
		}

		gettimeofday(&philo->state->curr_time, NULL);
		if (philo->state->still_alive && (gettime_usec(philo->state->curr_time) - gettime_usec(philo->meal_time)) > (unsigned long)(philo->state->time_to_die * 1000))
		{
			pthread_mutex_lock(philo->state->mutex);
			if (philo->state->still_alive != false)
			{
				philo->state->still_alive = false;
				philo->is_thinking = false;
				print_message(philo, "died");
			}
			pthread_mutex_unlock(philo->state->mutex);
			return (NULL);
		}

		// give forks back
		if (philo->state->still_alive && philo->forks_in_use == 2)
		{
			pthread_mutex_lock(philo->mutex);
			philo->fork_available = true;
			philo->next->fork_available = true;
			pthread_mutex_unlock(philo->mutex);
			philo->forks_in_use -= 2;
			philo->is_thinking = false;
			print_message(philo, "is sleeping");
			usleep(philo->state->time_to_sleep * 1000);
			if (!philo->state->still_alive)
				return (NULL);
		}
	}
	return (NULL);
}

void	connect_philo(t_philo *last, t_philo *new)
{
	last->next = new;
	new->prev = last;
}

/**
 * @param number order number of philosopher
 * @param mutex mutual mutex to lock fork availability while eating
*/
t_philo	*create_philo(int number, pthread_mutex_t *mutex, t_state *state)
{
	t_philo	*philo;

	philo = (t_philo *)malloc(sizeof(t_philo));
	if (!philo)
		return (NULL);
	philo->no = number;
	philo->next = NULL;
	philo->prev = NULL;
	philo->fork_available = true;
	philo->forks_in_use = 0;
	if (gettimeofday(&philo->meal_time, NULL) != EXIT_SUCCESS)
		return (NULL);
	philo->eat_count = 0;
	philo->mutex = mutex;
	philo->state = state;
	return (philo);
}

int	init_philos(t_state *state, pthread_mutex_t *mutex)
{
	t_philo	*curr;
	int i;

	state->head = create_philo(1, mutex, state);
	if (!state->head)
		return (EXIT_FAILURE);
	curr = state->head;
	i = 1;
	while (i < state->number_of_philosophers)
	{
		curr->next = create_philo(i + 1, mutex, state);
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

int	init_state(t_state *state, char *argv[],
	pthread_mutex_t *state_mutex, pthread_mutex_t *meal_mutex)
{
	state->number_of_philosophers = ft_atoi(argv[1]);
	state->time_to_die = ft_atoi(argv[2]);
	state->time_to_eat = ft_atoi(argv[3]);
	state->time_to_sleep = ft_atoi(argv[4]);
	if (!argv[5])
		state->meals_per_philo = -1;
	else
		state->meals_per_philo = ft_atoi(argv[5]);
	state->meals_left = ft_atoi(argv[1]) * state->meals_per_philo;
	if (pthread_mutex_init(state_mutex, NULL) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (pthread_mutex_init(meal_mutex, NULL) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	state->still_alive = true;
	state->mutex = state_mutex;
	state->meal_mutex = meal_mutex;
	return (EXIT_SUCCESS);
}

int	main(int argc, char const *argv[])
{
	pthread_mutex_t mutex;
	pthread_mutex_t state_mutex;
	pthread_mutex_t meal_mutex;
	t_state			state;

	if (argc < 5 || argc > 6)
		return (EXIT_FAILURE);
	if (init_state(&state, argv, &state_mutex, &meal_mutex) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (pthread_mutex_init(&mutex, NULL) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (init_philos(&state, &mutex) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (create_philo_threads(&state) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (join_philo_threads(&state) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (pthread_mutex_destroy(&mutex) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (pthread_mutex_destroy(&state_mutex) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (pthread_mutex_destroy(&meal_mutex) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	free_philos(state.head);
	return (EXIT_SUCCESS);
}
