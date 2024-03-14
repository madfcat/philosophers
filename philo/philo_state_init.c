/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 12:50:24 by vshchuki          #+#    #+#             */
/*   Updated: 2024/03/14 12:55:35 by vshchuki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	check_arg(const char *str)
{
	char	*converted;
	int		result;

	converted = ft_itoa(ft_atoi(str));
	result = ft_strncmp(str, converted, ft_strlen(str) + 1);
	free(converted);
	return (result);
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