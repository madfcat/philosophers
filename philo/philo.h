/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 15:27:01 by vshchuki          #+#    #+#             */
/*   Updated: 2024/02/26 02:07:11 by vshchuki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define FT_PIPEX_H
# include <unistd.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdio.h> // remove later

typedef enum e_bool {
    false = 0,
    true = 1
}	t_bool;

typedef struct s_philo t_philo;

typedef struct s_state {
	// struct timeval	tp;
	t_bool	still_alive;
	t_philo	*head;
}	t_state;

// fork_available is a philosopher's left fork 
typedef struct s_philo
{
	int				no;
	t_bool			fork_available;
	unsigned char	forks_in_use;
	struct timeval	meal_time;
	struct timeval	curr_time;
	pthread_t		id;
	struct s_philo	*next;
	struct s_philo	*prev;
	pthread_mutex_t	*mutex;
	t_state			*state;
}	t_philo;

void	free_philos(t_philo *head);

#endif