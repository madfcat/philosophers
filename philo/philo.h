/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 15:27:01 by vshchuki          #+#    #+#             */
/*   Updated: 2024/02/26 17:00:50 by vshchuki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <unistd.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdio.h>

typedef enum e_bool {
    false = 0,
    true = 1
}	t_bool;

typedef struct s_philo t_philo;

typedef struct s_state {
	t_bool			still_alive;
	t_philo			*head;
	pthread_mutex_t	*mutex;
	struct timeval	curr_time;
}	t_state;

// fork_available is a philosopher's left fork 
typedef struct s_philo
{
	int				no;
	t_bool			fork_available;
	unsigned char	forks_in_use;
	struct timeval	meal_time;
	pthread_t		id;
	struct s_philo	*next;
	struct s_philo	*prev;
	pthread_mutex_t	*mutex;
	t_state			*state;
	t_bool			is_thinking;
}	t_philo;

void	free_philos(t_philo *head);

#endif