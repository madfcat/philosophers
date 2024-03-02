/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 15:27:12 by vshchuki          #+#    #+#             */
/*   Updated: 2024/03/02 19:28:03 by vshchuki         ###   ########.fr       */
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

void *state_routine(void *arg)
{
	t_state	*state;
	state = (t_state *)arg;
	
	while(state->still_alive)
	{
		gettimeofday(&state->curr, NULL);
	}
	return (NULL);
}

int	take_forks_eat(t_philo *philo)
{
/* 	if (philo->fork_available && philo->next->fork_available)
	{ */
	pthread_mutex_t *first_fork;
	pthread_mutex_t *second_fork;
	
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

	pthread_mutex_lock(first_fork);
		philo->fork_available = false;
		philo->next->fork_available = false;
		print_message(philo, "has taken the first fork");
	pthread_mutex_lock(second_fork);
		print_message(philo, "has taken the second fork");
		if (!philo->state->still_alive)
		{
			pthread_mutex_unlock(&philo->fork_mutex);
			pthread_mutex_unlock(&philo->next->fork_mutex);
			return (EXIT_PHILO_DEATH);
		}
		print_message(philo, "is eating");
		philo->meal_time = philo->state->curr;
		if (thread_sleep(philo, philo->state->time_to_eat) == EXIT_PHILO_DEATH)
			return (EXIT_PHILO_DEATH);
		philo->next->fork_available = true;
	pthread_mutex_unlock(&philo->next->fork_mutex);
		philo->fork_available = true;
	pthread_mutex_unlock(&philo->fork_mutex);
/* 	} */
		print_message(philo, "is sleeping");
		if (thread_sleep(philo, philo->state->time_to_sleep) == EXIT_PHILO_DEATH)
			return (EXIT_PHILO_DEATH);
		print_message(philo, "is thinking");
		return (EXIT_SUCCESS);
}

/* void *eat(t_philo *philo)
{
	if (philo->state->still_alive && philo->forks_in_use == 2)
	{
		gettimeofday(&philo->meal_time, NULL);
		if (!philo->state->still_alive)
			return (NULL);
		pthread_mutex_lock(philo->state->meal_mutex);
		if (philo->state->meals_per_philo > 0)
		{
			philo->state->meals_left -= 1;
			if (!philo->state->meals_left)
			{
				pthread_mutex_lock(philo->state->mutex);
				philo->state->still_alive = false;
				pthread_mutex_unlock(philo->state->mutex);
			}
		}
		pthread_mutex_unlock(philo->state->meal_mutex);
		if (!philo->state->still_alive)
			return (NULL);
		philo->is_thinking = false;
		print_message(philo, "is eating");
		usleep(philo->state->time_to_eat * 1000);
		if (!philo->state->still_alive)
			return (NULL);

		if (philo->forks_in_use == 2)
		{
			pthread_mutex_lock(&philo->next->fork_mutex);
			pthread_mutex_lock(&philo->fork_mutex);
			philo->fork_available = true;
			philo->next->fork_available = true;
			philo->forks_in_use -= 2;
			pthread_mutex_unlock(&philo->fork_mutex);
			pthread_mutex_unlock(&philo->next->fork_mutex);
			print_message(philo, "is sleeping");
			usleep(philo->state->time_to_sleep * 1000);
			philo->is_thinking = false;
			if (!philo->state->still_alive)
				return (NULL);
		}
	}
	return (philo);
} */

// void	*check_death(t_philo *philo)
// {
// 	struct timeval curr_time;
// 	// pthread_mutex_lock(philo->state->curr_mutex);
	
// 	gettimeofday(&curr_time, NULL);
// 	// pthread_mutex_unlock(philo->state->curr_mutex);
// /* 	printf("%d: curr_time: %ld\n", philo->no, gettime_ms(curr_time));
// 	printf("%d: meal_time: %ld\n", philo->no, gettime_ms(philo->meal_time));
// 	printf("%d: passed: %ld\n", philo->no, gettime_ms(curr_time) - gettime_ms(philo->meal_time));
// 	printf("time to die: %d\n",philo->state->time_to_die); */
// 	if (philo->state->still_alive && (gettime_ms(curr_time) - gettime_ms(philo->meal_time)) > (unsigned long)(philo->state->time_to_die))
// 	{
// 		philo->is_thinking = false;
// 		pthread_mutex_lock(philo->state->mutex);
// 			philo->state->still_alive = false;
// 			print_message(philo, "died");
// 		pthread_mutex_unlock(philo->state->mutex);
// 		return (NULL);
// 	}
// 	return (philo);
// }

/* void	*give_forks_back(t_philo *philo)
{
	if (philo->state->still_alive && philo->forks_in_use == 2)
	{

		pthread_mutex_lock(&philo->next->fork_mutex);
		pthread_mutex_lock(&philo->fork_mutex);
		philo->fork_available = true;
		philo->forks_in_use -= 1;
		philo->next->fork_available = true;
		philo->forks_in_use -= 1;
		pthread_mutex_unlock(&philo->fork_mutex);
		pthread_mutex_unlock(&philo->next->fork_mutex);

		philo->is_thinking = false;
		print_message(philo, "is sleeping");
		usleep(philo->state->time_to_sleep * 1000);
		if (!philo->state->still_alive)
			return (NULL);
	}
	return (philo);
} */

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
		// take forks
		// printf("philo %d routine\n", philo->no);
		if (philo->state->still_alive && take_forks_eat(philo) == EXIT_PHILO_DEATH)
		{
			// printf("philo died!\n");
			return (NULL);
		}
		if (!philo->state->still_alive)
			return (NULL);
		// eat 
/* 		if (!eat(philo))
			return (NULL); */
/* 		if (!philo->is_thinking)
		{
			print_message(philo, "is thinking");
			philo->is_thinking = true;
		} */
		// check death
/* 		if (!check_death(philo))
			return (NULL); */
		// give forks back
/* 		if (!give_forks_back(philo))
			return (NULL); */
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
	gettimeofday(&state->start, NULL);
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

	err = pthread_create(&state.id, NULL, &state_routine, &state);
		if (err != 0)
			return (err);
	if (init_philos(&state) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (create_philo_threads(&state) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (join_philo_threads(&state) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	// state.still_alive = false;
	err = pthread_join(state.id, NULL);
		if (err != 0)
			return (err);
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
