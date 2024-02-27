/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 15:27:01 by vshchuki          #+#    #+#             */
/*   Updated: 2024/02/27 20:23:44 by vshchuki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <unistd.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdio.h>
     #include <string.h> // remove me

typedef enum e_bool {
    false = 0,
    true = 1
}	t_bool;

typedef struct s_philo t_philo;

typedef struct s_state {
	int	number_of_philosophers;
	int	time_to_die;
	int	time_to_eat;
	int	time_to_sleep;
	int meals_per_philo;
	int meals_left;

	t_bool			still_alive;
	t_philo			*head;
	
	pthread_mutex_t	*mutex;
	pthread_mutex_t	*meal_mutex;
	pthread_mutex_t	*curr_mutex;

	// struct timeval	curr_time;
}	t_state;

// fork_available is a philosopher's left fork 
typedef struct s_philo
{
	int				no;
	pthread_t		id;
	struct s_philo	*next;
	struct s_philo	*prev;
	
	t_bool			fork_available;
	unsigned char	forks_in_use;
	struct timeval	meal_time;
	unsigned int	eat_count;
	t_bool			is_thinking;

	pthread_mutex_t	*mutex;
	t_state			*state;
}	t_philo;

unsigned long	gettime_usec(struct timeval time);
int				print_message(t_philo *philo, char *msg);
void			free_philos(t_philo *head);

int				ft_atoi(const char *str);

#endif