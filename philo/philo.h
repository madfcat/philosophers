/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 15:27:01 by vshchuki          #+#    #+#             */
/*   Updated: 2024/03/14 02:18:29 by vshchuki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


// might need to remove EXIT_OTHER_DEATH
#ifndef PHILO_H
# define PHILO_H
# ifndef EXIT_PHILO_DETH
#  define EXIT_PHILO_DEATH -1
# endif
# ifndef EXIT_OTHER_DEATH
#  define EXIT_OTHER_DEATH -2
# endif
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

// typedef enum e_status {
// 	thinking = 1,
// 	eating = 2,
// 	sleeping = 3
// }	t_status;

typedef struct s_philo t_philo;

typedef struct s_state {
	pthread_t		id;
	int				number_of_philosophers;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int 			meals_per_philo;
	int 			meals_left;

	int				died_first;

	t_philo			*head;

	struct timeval	start_time;

	pthread_mutex_t	*death_mutex;
	pthread_mutex_t	*meal_mutex;
}	t_state;

// fork_available is a philosopher's left fork 
typedef struct s_philo
{
	int				no;
	pthread_t		id;
	struct s_philo	*next;
	struct s_philo	*prev;

	struct timeval	meal_time;

	int				meals_count;

	t_bool			fork_available;
	// char			forks_count;

	pthread_mutex_t	fork_mutex;
	pthread_mutex_t	status_mutex;
	t_state			*state;
}	t_philo;

int				check_arg(const char *str);

unsigned long	gettime_usec(struct timeval time);
unsigned long	gettime_ms(struct timeval time);
int				print_message(t_philo *philo, char *msg);
int				free_philos(t_philo *head);

int				thread_sleep(t_philo *philo, unsigned long (*f)(struct timeval), unsigned long time, useconds_t step);
int				thread_sleep_usec(t_philo *philo, int usec);

int				check_alive(t_philo *philo);
int				check_meal(t_philo *philo);

int				ft_atoi(const char *str);
char			*ft_itoa(int n);
int				ft_strncmp(const char *s1, const char *s2, unsigned int n);
int				ft_strlen(const char *str);

#endif