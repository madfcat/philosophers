/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 15:27:12 by vshchuki          #+#    #+#             */
/*   Updated: 2024/02/26 01:37:55 by vshchuki         ###   ########.fr       */
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

int	number_of_philosophers = 6;
int	time_to_die = 110;
int	time_to_eat = 50;
int	time_to_sleep = 50;
// number_of_times_each_philosopher_must_eat

#include "philo.h"
// usleep()

unsigned long gettime_usec(struct timeval time)
{
	return (time.tv_sec * 1000000 + time.tv_usec);
}

void *routine(void	*arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	gettimeofday(&philo->meal_time, NULL);

	// printf("philo %lu is thinking!\n", (unsigned long)philo->id);
	printf("philo %d is thinking!\n", philo->no);
	// printf("philo %d has not eaten since %ld\n", philo->no, gettime_usec(philo->meal_time));
		
	while(philo->state->still_alive)
	{
		// take forks
		if (philo->state->still_alive && philo->fork_available && philo->next->fork_available)
		{
			pthread_mutex_lock(philo->mutex);
			philo->fork_available = false;
			philo->next->fork_available = false;
			philo->forks_in_use += 2;
			pthread_mutex_unlock(philo->mutex);
		}

		// eat 
		// printf("philo %lu is eating!\n", (unsigned long)philo->id);
		if (philo->state->still_alive && philo->forks_in_use == 2) {
			gettimeofday(&philo->meal_time, NULL);
			printf("philo %d is eating!\n", philo->no);
			usleep(time_to_eat * 1000);
		}

		gettimeofday(&philo->curr_time, NULL);
		// printf("philo %d last meal was %lu ago\n", philo->no, gettime_usec(philo->curr_time) - gettime_usec(philo->meal_time));
		if (philo->state->still_alive && (gettime_usec(philo->curr_time) - gettime_usec(philo->meal_time)) > (unsigned long)(time_to_die * 1000))
		{
			printf("philo %d last meal was %lu ago\n", philo->no, gettime_usec(philo->curr_time) - gettime_usec(philo->meal_time));
			printf("philo %d has died\n", philo->no);
			philo->state->still_alive = false;
			return (NULL);
		}

/* 		gettimeofday(&philo->curr_time, NULL);
		if (philo->state->still_alive && philo->forks_in_use == 2)
		{
			printf("It took philo %d %ld microseconds to finish eating\n", philo->no, gettime_usec(philo->curr_time) - gettime_usec(philo->meal_time));
		} */


		// give forks back
		if (philo->state->still_alive && philo->forks_in_use == 2)
		{
			pthread_mutex_lock(philo->mutex);
			philo->fork_available = true;
			philo->next->fork_available = true;
			philo->forks_in_use -= 2;
			pthread_mutex_unlock(philo->mutex);

			// printf("philo %lu is sleeping!\n", (unsigned long)philo->id);
			printf("philo %d is sleeping!\n", philo->no);
			usleep(time_to_sleep * 1000);

			printf("philo %d is thinking!\n", philo->no);
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
	philo->no = number;
	philo->fork_available = true;
	philo->forks_in_use = 0;
	gettimeofday(&philo->meal_time, NULL);
	philo->next = NULL;
	philo->prev = NULL;
	philo->mutex = mutex;
	philo->state = state;
	return (philo);
}

int	main(int argc, char const *argv[])
{
	pthread_mutex_t mutex;
	t_state			state;
	t_philo			*curr;
	// t_philo			*prev;
	// t_philo			**philos;
	int 			i;
	pthread_mutex_init(&mutex, NULL);
	(void)argc;
	(void)argv;

	state.still_alive = true;

	// 1. init linked list philosphers here creating threads and running loop for each philosopher
	// philos = (t_philo **)malloc((number_of_philosophers + 1) * sizeof(t_philo *));
	// philos[number_of_philosophers] = NULL;

	// philos[0] = create_philo(1, &mutex, &state);
	state.head = create_philo(1, &mutex, &state);
	curr = state.head;
	i = 1;
	while (i < number_of_philosophers)
	{
		curr->next = create_philo(i + 1, &mutex, &state);
		connect_philo(curr, curr->next);
		// prev = curr;
		curr = curr->next;
		i++;
	}
	connect_philo(curr, state.head);
 
	// each philosopher has a loop think eat sleep
	curr = state.head;
	// while (curr->no <= number_of_philosophers)
	while (curr->next != state.head)
	{
		pthread_create(&curr->id, NULL, &routine, curr);
		curr = curr->next;
	}
	pthread_create(&curr->id, NULL, &routine, curr);
	
	// 2. join threads in a loop
	curr = state.head;
	// while (curr->no <= number_of_philosophers)
	while (curr->next != state.head)
	{
		pthread_join(curr->id, NULL);
		curr = curr->next;
	}
	pthread_join(curr->id, NULL);

	pthread_mutex_destroy(&mutex);
	return (0);
}
