/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 15:27:12 by vshchuki          #+#    #+#             */
/*   Updated: 2024/03/14 02:45:05 by vshchuki         ###   ########.fr       */
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
 * @return 0 if noone died, otherwise, should return philo number
*/
int	check_alive(t_philo *philo)
{
	int	result;

	pthread_mutex_lock(philo->state->death_mutex);
	result = philo->state->died_first;
	pthread_mutex_unlock(philo->state->death_mutex);
	return (result);
}

t_bool	check_fork(t_philo *philo)
{
	int	result;

	pthread_mutex_lock(&philo->fork_mutex);
	result = philo->fork_available;
	pthread_mutex_unlock(&philo->fork_mutex);
	return (result);
}

int	check_meal(t_philo *philo)
{
	int	result;

	pthread_mutex_lock(philo->state->meal_mutex);
	result = philo->state->meals_left;
	pthread_mutex_unlock(philo->state->meal_mutex);
	return (result);
}

int	handle_meals(t_philo *philo)
{
	pthread_mutex_lock(philo->state->meal_mutex);
	if (philo->state->meals_left > 0
		&& philo->meals_count != philo->state->meals_per_philo)
		philo->state->meals_left -= 1;
	else if (philo->state->meals_left == 0)
	{
		pthread_mutex_lock(philo->state->death_mutex);
		philo->state->died_first = philo->no;
		pthread_mutex_unlock(philo->state->death_mutex);
		pthread_mutex_unlock(philo->state->meal_mutex);
		return (EXIT_PHILO_DEATH);
	}
	pthread_mutex_unlock(philo->state->meal_mutex);
	philo->meals_count += 1;
	return (EXIT_SUCCESS);
}

int	eat(t_philo *philo)
{
	struct timeval	curr_time;
	int				err;

	err = gettimeofday(&curr_time, NULL);
	if (err != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	philo->meal_time = curr_time;
	print_message(philo, "is eating");
	if (philo->state->meals_per_philo > -1 && handle_meals(philo)
		== EXIT_PHILO_DEATH)
		return (EXIT_PHILO_DEATH);
	if (thread_sleep(philo, gettime_ms, philo->state->time_to_eat, 500)
		== EXIT_PHILO_DEATH)
	{
		pthread_mutex_lock(philo->state->death_mutex);
		if (philo->state->died_first == 0)
			philo->state->died_first = philo->no;
		pthread_mutex_unlock(philo->state->death_mutex);
		pthread_mutex_lock(&philo->next->fork_mutex);
		philo->next->fork_available = true;
		pthread_mutex_unlock(&philo->next->fork_mutex);
		pthread_mutex_lock(&philo->fork_mutex);
		philo->fork_available = true;
		pthread_mutex_unlock(&philo->fork_mutex);
		return (EXIT_PHILO_DEATH);
	}
	return (EXIT_SUCCESS);
}

void fork_back(t_philo *philo)
{
	pthread_mutex_lock(&philo->fork_mutex);
	philo->fork_available = true;
	pthread_mutex_unlock(&philo->fork_mutex);
	// philo->forks_count -= 1;
}

int first_fork(t_philo *philo)
{
	pthread_mutex_lock(&philo->fork_mutex);
	if (philo->fork_available)
	{
		philo->fork_available = false;
		// philo->forks_count += 1;
	}
	else
	{
		pthread_mutex_unlock(&philo->fork_mutex);
		return (EXIT_FAILURE);
	}
	pthread_mutex_unlock(&philo->fork_mutex);
	print_message(philo, "has taken the first fork");
	return (EXIT_SUCCESS);
}

int second_fork(t_philo *philo)
{
	if (philo->no != philo->next->no)
	{
		while (!check_fork(philo->next))
		{
			if (thread_sleep(philo, gettime_usec, 1, 100) == EXIT_PHILO_DEATH)
				return (EXIT_PHILO_DEATH);
		}
		pthread_mutex_lock(&philo->next->fork_mutex);
		philo->next->fork_available = false;
		// philo->forks_count += 1;
		pthread_mutex_unlock(&philo->next->fork_mutex);
		print_message(philo, "has taken the second fork");
		return (EXIT_SUCCESS);
	}
	return (EXIT_FAILURE);
}

int	take_forks_eat(t_philo *philo)
{
	int second_res;

	if (thread_sleep(philo, gettime_usec, 1, 1) == EXIT_PHILO_DEATH)
		return (EXIT_PHILO_DEATH);
	if (first_fork(philo) == EXIT_FAILURE)
		return (EXIT_FAILURE);
	second_res = second_fork(philo);
	if (second_res != EXIT_SUCCESS)
	{
		return (second_res);
	}
	if (eat(philo) == EXIT_PHILO_DEATH)
		return (EXIT_PHILO_DEATH);
	fork_back(philo);
	fork_back(philo->next);
	print_message(philo, "is sleeping");
	if (thread_sleep(philo, gettime_ms, philo->state->time_to_sleep, 500)
		== EXIT_PHILO_DEATH)
		return (EXIT_PHILO_DEATH);
	print_message(philo, "is thinking");
	while (!check_fork(philo))
	{
		if (thread_sleep(philo, gettime_usec, 1, 100) == EXIT_PHILO_DEATH)
			return (EXIT_PHILO_DEATH);
	}
	return (EXIT_SUCCESS);
}

/**
 * @return NULL on success
*/
void *routine(void *arg)
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

int	check_args(char const *argv[])
{
	int	i;

	i = 1;
	while (argv[i])
	{
		if (check_arg(argv[i]))
		{
			write(2, "Wrong argument format\n", 23);
			return (EXIT_FAILURE);
		}
		i++;
	}
	return (EXIT_SUCCESS);
}

int	init_state(t_state *state, char const *argv[],
	pthread_mutex_t *state_mutex, pthread_mutex_t *meal_mutex)
{
	if (check_args(argv) == EXIT_FAILURE)
		return (EXIT_FAILURE);
	state->number_of_philosophers = ft_atoi(argv[1]);
	state->time_to_die = ft_atoi(argv[2]);
	state->time_to_eat = ft_atoi(argv[3]);
	state->time_to_sleep = ft_atoi(argv[4]);
	if (!argv[5])
		state->meals_per_philo = -1;
	else
		state->meals_per_philo = ft_atoi(argv[5]);
	state->meals_left = ft_atoi(argv[1]) * state->meals_per_philo;
	if (state->meals_left == 0)
		return (EXIT_FAILURE);
	if (pthread_mutex_init(state_mutex, NULL) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (pthread_mutex_init(meal_mutex, NULL) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	state->died_first = 0;
	state->death_mutex = state_mutex;
	state->meal_mutex = meal_mutex;
	return (EXIT_SUCCESS);
}

int	main(const int argc, char const *argv[])
{
	pthread_mutex_t	state_mutex;
	pthread_mutex_t	meal_mutex;
	t_state			state;

	if (argc < 5 || argc > 6)
		return (EXIT_FAILURE);
	if (init_state(&state, argv, &state_mutex, &meal_mutex) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (init_philos(&state) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (create_philo_threads(&state) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (join_philo_threads(&state) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (pthread_mutex_destroy(&state_mutex) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (pthread_mutex_destroy(&meal_mutex) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (free_philos(state.head) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}
