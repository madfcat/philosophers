/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 15:27:12 by vshchuki          #+#    #+#             */
/*   Updated: 2024/02/25 19:25:56 by vshchuki         ###   ########.fr       */
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

int	number_of_philosophers = 5;
int	time_to_die = 17000;
int	time_to_eat = 5000;
int	time_to_sleep = 5000;
// number_of_times_each_philosopher_must_eat

#include "philo.h"
// usleep()

void *routine(void	*arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	gettimeofday(&philo->meal_time, NULL);

	// printf("philo %lu is thinking!\n", (unsigned long)philo->id);
	printf("philo %d is thinking!\n", philo->no);
	printf("philo %d has not eaten since %ld\n", philo->no, philo->meal_time.tv_sec);
		
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
		if (philo->state->still_alive && (philo->curr_time.tv_sec - philo->meal_time.tv_sec) * 1000 > time_to_die)
		{
			printf("philo %d has died\n", philo->no);
			philo->state->still_alive = false;
			return (NULL);
		}

		gettimeofday(&philo->curr_time, NULL);
		if (philo->state->still_alive && philo->forks_in_use == 2)
		{
			printf("It took philo %d %ld seconds to finish eating\n", philo->no, philo->curr_time.tv_sec - philo->meal_time.tv_sec);
		}


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
/* 	t_philo			**philos;
	int 			i; */
	pthread_mutex_init(&mutex, NULL);
	(void)argc;
	(void)argv;

	state.still_alive = true;

	// 1. init linked list philosphers here creating threads and running loop for each philosopher
/* 	philos = (t_philo **)malloc((number_of_philosophers + 1) * sizeof(t_philo *));
	philos[number_of_philosophers] = NULL;

	i = 0;
	philos[i] = create_philo(i + 1, &mutex, &state);
	while (i < number_of_philosophers)
	{
		philos[i] = create_philo(i + 1, &mutex, &state);
		connect_philo(philos[i - 1], philos[i]);
	}
	connect_philo(philos[i - 1], philos[0]); */

	t_philo *philo1;
	philo1 = create_philo(1, &mutex, &state);
	t_philo *philo2;
	philo2 = create_philo(2, &mutex, &state);
	connect_philo(philo1, philo2);
	t_philo *philo3;
	philo3 = create_philo(3, &mutex, &state);
	connect_philo(philo2, philo3);
	t_philo *philo4;
	philo4 = create_philo(4, &mutex, &state);
	connect_philo(philo3, philo4);
	t_philo *philo5;
	philo5 = create_philo(5, &mutex, &state);
	connect_philo(philo4, philo5);
	//connect last to first
	connect_philo(philo5, philo1);


	// each philosopher has a loop think eat sleep 
/* 	i = 0;
	while (i < number_of_philosophers)
	{
		pthread_create(&philos[i]->id, NULL, &routine, philos[i]);
	} */
	pthread_create(&philo1->id, NULL, &routine, philo1);
	pthread_create(&philo2->id, NULL, &routine, philo2);
	pthread_create(&philo3->id, NULL, &routine, philo3);
	pthread_create(&philo4->id, NULL, &routine, philo4);
	pthread_create(&philo5->id, NULL, &routine, philo5);
	// 2. join threads in a loop
/* 	i = 0;
	while (i < number_of_philosophers)
	{
	pthread_join(philos[i]->id, NULL);
	} */
	pthread_join(philo1->id, NULL);
	pthread_join(philo2->id, NULL);
	pthread_join(philo3->id, NULL);
	pthread_join(philo4->id, NULL);
	pthread_join(philo5->id, NULL);
	pthread_mutex_destroy(&mutex);
	return (0);
}
