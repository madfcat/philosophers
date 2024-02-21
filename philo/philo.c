/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 15:27:12 by vshchuki          #+#    #+#             */
/*   Updated: 2024/02/21 19:12:23 by vshchuki         ###   ########.fr       */
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
int	time_to_die = 5000;
int	time_to_eat = 5000;
int	time_to_sleep = 5000;
// number_of_times_each_philosopher_must_eat

#include "philo.h"
// usleep()

void *routine(void	*arg) {
	t_philo *philo = (t_philo *)arg;
	printf("philo %d runs!\n", (int)philo->id);
	usleep(5000000);
	printf("5 seconds passed");
	return (NULL);
}

t_philo	*init_philo(void)
{
	t_philo	*philo;

	philo = (t_philo *)malloc(sizeof(t_philo));
	philo->no = 1;
	philo->left_fork_available = true;
	gettimeofday(&philo->not_eaten_since, NULL);
	// (pthread_t **)malloc(sizeof(pthread_t *));
	return (philo);
}

int	main(int argc, char const *argv[])
{
	(void)argc;
	(void)argv;

	// t_philo **philos = (t_philo *)malloc((number_of_philosophers + 1) * sizeof(t_philo *));
	// philos[number_of_philosophers] = NULL;
	t_philo *philo1;
	philo1 = init_philo();
	pthread_create(&philo1->id, NULL, &routine, &philo1);
	pthread_join(philo1->id, NULL);
	return (0);
}
