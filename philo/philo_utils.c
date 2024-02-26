/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 01:52:52 by vshchuki          #+#    #+#             */
/*   Updated: 2024/02/26 22:24:48 by vshchuki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"


unsigned long	gettime_usec(struct timeval time)
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

void	free_philos(t_philo *head)
{
	t_philo *prev;
	t_philo *curr;

	curr = head;
	while (curr)
	{
		prev = curr;
		curr = curr->next;
		free(prev);
		if (curr == head)
			break;
	}
}