/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 15:27:01 by vshchuki          #+#    #+#             */
/*   Updated: 2024/03/14 22:46:35 by vshchuki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H
# ifndef EXIT_PHILO_DEATH
#  define EXIT_PHILO_DEATH -1
# endif
# include <unistd.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdio.h>

typedef enum e_bool
{
	false = 0,
	true = 1
}	t_bool;

typedef struct s_philo	t_philo;

typedef struct s_state
{
	int				number_of_philosophers;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				meals_per_philo;
	int				meals_left;

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
	pthread_mutex_t	fork_mutex;

	t_state			*state;
}	t_philo;

int				init_state(t_state *state, char const *argv[],
					pthread_mutex_t *state_mutex, pthread_mutex_t *meal_mutex);
int				check_arg(const char *str);

int				init_philos(t_state *state);
int				create_philo_threads(t_state *state);
int				join_philo_threads(t_state *state);

int				take_forks_eat(t_philo *philo);
int				eat(t_philo *philo);

unsigned long	gettime_usec(struct timeval time);
unsigned long	gettime_ms(struct timeval time);
int				print_message(t_philo *philo, char *msg);
int				free_philos(t_philo *head);

int				thread_sleep(t_philo *philo,
					unsigned long (*f)(struct timeval),
					unsigned long time, useconds_t step);
int				thread_sleep_usec(t_philo *philo, int usec);

int				check_alive(t_philo *philo);
int				check_meal(t_philo *philo);

int				ft_atoi(const char *str);
char			*ft_itoa(int n);
int				ft_strncmp(const char *s1, const char *s2, unsigned int n);
int				ft_strlen(const char *str);

#endif