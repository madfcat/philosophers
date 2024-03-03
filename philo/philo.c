/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 15:27:12 by vshchuki          #+#    #+#             */
/*   Updated: 2024/03/03 16:41:47 by vshchuki         ###   ########.fr       */
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

/* void *state_routine(void *arg)
{
	t_state			*state;
	unsigned long	error;
	
	error = EXIT_FAILURE;
	state = (t_state *)arg;
	while(state->still_alive)
	{
		error = gettimeofday(&state->curr_time, NULL);
		if (error != EXIT_SUCCESS)
			return (void *)error;
	}
	return (NULL);
} */

int	take_forks_eat(t_philo *philo)
{
	pthread_mutex_t *first_fork;
	pthread_mutex_t *second_fork;
	struct timeval	curr_time;
	int 			err;
	
	// printf("philo %d take\n", philo->no);
	if (philo->no % 2 == 1)
	{
		first_fork = &philo->fork_mutex;
		second_fork = &philo->next->fork_mutex;
	}
	else
	{
		first_fork = &philo->next->fork_mutex;
		second_fork = &philo->fork_mutex;
	}
/* 	if (philo->next->status != eating && philo->prev->status != eating)
	{ */
	pthread_mutex_lock(first_fork);
		print_message(philo, "has taken the first fork");
	pthread_mutex_lock(second_fork);
		print_message(philo, "has taken the second fork");
		if (!philo->state->still_alive)
		{
			pthread_mutex_unlock(&philo->fork_mutex);
			pthread_mutex_unlock(&philo->next->fork_mutex);
			return (EXIT_PHILO_DEATH);
		}

/* 		pthread_mutex_lock(philo->state->meal_mutex);
		if (philo->state->meals_per_philo > 0)
		{
			philo->state->meals_left -= 1;
			printf("meals left: %d\n", philo->state->meals_left);
		}
		pthread_mutex_unlock(philo->state->meal_mutex);
		if (philo->state->meals_per_philo == 0)
		{
			pthread_mutex_unlock(&philo->next->fork_mutex);
			pthread_mutex_unlock(&philo->fork_mutex);
			return (EXIT_OTHER_DEATH);
		} */

		err = gettimeofday(&curr_time, NULL);
		if (err != EXIT_SUCCESS)
			return (EXIT_FAILURE);
/* this part check death of thinking philo, the problem is that check is after it took the forks,
but should happen earlier

 		if (gettime_ms(curr_time) - gettime_ms(philo->meal_time) >= (unsigned long)philo->state->time_to_die)
		{
			pthread_mutex_unlock(&philo->next->fork_mutex);
			pthread_mutex_unlock(&philo->fork_mutex);
			philo->state->still_alive = false;
			return (EXIT_PHILO_DEATH);
		}*/

		philo->meal_time = curr_time;
		philo->status = eating;

		print_message(philo, "is eating");
		if (thread_sleep(philo, philo->state->time_to_eat) == EXIT_PHILO_DEATH)
		{
			pthread_mutex_unlock(&philo->next->fork_mutex);
			pthread_mutex_unlock(&philo->fork_mutex);
			philo->state->still_alive = false;
			return (EXIT_PHILO_DEATH);
		}
	pthread_mutex_unlock(&philo->next->fork_mutex);
	pthread_mutex_unlock(&philo->fork_mutex);
/* 	}
		if (philo->status == eating)
		{ */
		philo->status = sleeping;
		print_message(philo, "is sleeping");
		if (thread_sleep(philo, philo->state->time_to_sleep) == EXIT_PHILO_DEATH)
			return (EXIT_PHILO_DEATH);
		philo->status = thinking;
		print_message(philo, "is thinking");
/* 		} */

		// check death
		err = gettimeofday(&curr_time, NULL);
		if (err != EXIT_SUCCESS)
			return (EXIT_FAILURE);
 		if (gettime_ms(curr_time) - gettime_ms(philo->meal_time) >= (unsigned long)philo->state->time_to_die)
		{
			pthread_mutex_lock(philo->state->mutex);
			if (philo->state->still_alive)
				print_message(philo, "died");
			philo->state->still_alive = false;
			pthread_mutex_unlock(philo->state->mutex);
			return (EXIT_PHILO_DEATH);
		}

		return (EXIT_SUCCESS);
}

/**
 * @return NULL on success
*/
void *routine(void	*arg)
{
	t_philo	*philo;
	unsigned long error;
	int result;

	error = EXIT_FAILURE;
	philo = (t_philo *)arg;
	if (gettimeofday(&philo->meal_time, NULL) != EXIT_SUCCESS)
		return (void *)error;
	while(philo->state->still_alive)
	{
		// take forks
		// printf("philo %d routine\n", philo->no);
		result = take_forks_eat(philo) == EXIT_PHILO_DEATH;
		if (philo->state->still_alive && result == EXIT_PHILO_DEATH)
			return (NULL);
		if (!philo->state->still_alive)
			return (NULL);
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
*/
t_philo	*create_philo(int number, t_state *state)
{
	t_philo	*philo;

	philo = (t_philo *)malloc(sizeof(t_philo));
	if (!philo)
		return (NULL);
	if (pthread_mutex_init(&philo->fork_mutex, NULL) != EXIT_SUCCESS)
		return (NULL);
	philo->no = number;
	philo->next = NULL;
	philo->prev = NULL;
	philo->status = thinking;
	// philo->fork_available = true;
	// philo->is_thinking = true;
	// philo->forks_in_use = 0;
/*  	if (number == 1)
	{
		philo->fork_available = false;
		philo->is_thinking = false;
		philo->forks_in_use = 2;
	}
	if (number == 2)
	{
		philo->fork_available = false;
	} */

	if (gettimeofday(&philo->meal_time, NULL) != EXIT_SUCCESS)
		return (NULL);

	philo->state = state;
	return (philo);
}

int	init_philos(t_state *state)
{
	t_philo	*curr;
	int i;

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

int	init_state(t_state *state, char const *argv[],
	pthread_mutex_t *state_mutex, pthread_mutex_t *meal_mutex)
{
	state->number_of_philosophers = ft_atoi(argv[1]);
	state->time_to_die = ft_atoi(argv[2]);
	state->time_to_eat = ft_atoi(argv[3]);
	state->time_to_sleep = ft_atoi(argv[4]);
	gettimeofday(&state->start_time, NULL);
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
	// pthread_mutex_t mutex;
	pthread_mutex_t state_mutex;
	pthread_mutex_t meal_mutex;
	pthread_mutex_t curr_mutex;
	t_state			state;
	int				err;

	if (argc < 5 || argc > 6)
		return (EXIT_FAILURE);
	if (init_state(&state, argv, &state_mutex, &meal_mutex) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	state.curr_mutex = &curr_mutex;
	if (pthread_mutex_init(&curr_mutex, NULL) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	// if (pthread_mutex_init(&mutex, NULL) != EXIT_SUCCESS)
	// 	return (EXIT_FAILURE);

/* 	err = pthread_create(&state.id, NULL, &state_routine, &state);
		if (err != EXIT_SUCCESS)
			return (EXIT_FAILURE); */

	if (init_philos(&state) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (create_philo_threads(&state) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (join_philo_threads(&state) != EXIT_SUCCESS)
		return (EXIT_FAILURE);

	err = pthread_join(state.id, NULL);
		if (err != EXIT_SUCCESS)
			return (EXIT_FAILURE);

	if (pthread_mutex_destroy(&state_mutex) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (pthread_mutex_destroy(&meal_mutex) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (pthread_mutex_destroy(&curr_mutex) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (free_philos(state.head) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}
