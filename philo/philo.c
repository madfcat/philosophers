/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 15:27:12 by vshchuki          #+#    #+#             */
/*   Updated: 2024/02/26 22:06:37 by vshchuki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


/*
Arguments:
number_of_philosophers time_to_die time_to_eat
time_to_sleep
[number_of_times_each_philosopher_must_eat]
*/


/*
Allowed funcitons:
memset, printf, malloc, free, write,
usleep, gettimeofday, pthread_create,
pthread_detach, pthread_join, pthread_mutex_init,
pthread_mutex_destroy, pthread_mutex_lock,
pthread_mutex_unlock
*/

int	number_of_philosophers = 17;
int	time_to_die = 200;
int	time_to_eat = 70;
int	time_to_sleep = 50;
// number_of_times_each_philosopher_must_eat

#include "philo.h"

unsigned long gettime_usec(struct timeval time)
{
	return (time.tv_sec * 1000000 + time.tv_usec);
}

int	print_message(t_philo *philo, char *msg)
{
	if (gettimeofday(&philo->state->curr_time, NULL) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (printf("%lu %2d %s\n",
		gettime_usec(philo->state->curr_time) / 1000, philo->no, msg) < 0)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

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
			usleep(time_to_eat * 1000);
		}

		gettimeofday(&philo->state->curr_time, NULL);
		if (philo->state->still_alive && (gettime_usec(philo->state->curr_time) - gettime_usec(philo->meal_time)) > (unsigned long)(time_to_die * 1000))
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
			usleep(time_to_sleep * 1000);
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
	philo->fork_available = true;
	philo->forks_in_use = 0;
	if (gettimeofday(&philo->meal_time, NULL) != EXIT_SUCCESS)
		;
		// return (NULL);
	philo->next = NULL;
	philo->prev = NULL;
	philo->mutex = mutex;
	philo->state = state;
	return (philo);
}

int	main(int argc, char const *argv[])
{
	pthread_mutex_t mutex;
	pthread_mutex_t state_mutex;
	t_state			state;
	t_philo			*curr;
	int 			i;
	int				err;

	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&state_mutex, NULL);
	(void)argc;
	(void)argv;

	state.still_alive = true;
	state.mutex = &state_mutex;

	// 1. init linked list philosphers here creating threads and running loop for each philosopher

	state.head = create_philo(1, &mutex, &state);
	if (!state.head)
		return (EXIT_FAILURE);
	curr = state.head;
	i = 1;
	while (i < number_of_philosophers)
	{
		curr->next = create_philo(i + 1, &mutex, &state);
		if (!curr->next)
			return (EXIT_FAILURE);
		connect_philo(curr, curr->next);
		curr = curr->next;
		i++;
	}
	connect_philo(curr, state.head);
 
	/**
	 * Create philosophers threads
	*/
	curr = state.head;
	while (curr->next != state.head)
	{
		err = pthread_create(&curr->id, NULL, &routine, curr);
		if (err != 0)
			return (err);
		curr = curr->next;
	}
	err = pthread_create(&curr->id, NULL, &routine, curr);
		if (err != 0)
			return (err);
	
	/**
	 * Join philosophers threads
	*/
	curr = state.head;
	while (curr->next != state.head)
	{
		err = pthread_join(curr->id, NULL);
		if (err != 0)
			return (err);
		curr = curr->next;
	}
	err = pthread_join(curr->id, NULL);
	if (err != 0)
		return (err);

	err = pthread_mutex_destroy(&mutex);
	if (err != 0)
		return (err);
	free_philos(state.head);
	return (EXIT_SUCCESS);
}
