/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 15:27:01 by vshchuki          #+#    #+#             */
/*   Updated: 2024/02/21 19:09:32 by vshchuki         ###   ########.fr       */
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

typedef struct s_state {
	struct timeval	tp;
}	t_state;

typedef struct s_philo
{
	int				no;
	int				left_fork_available;
	struct timeval	not_eaten_since;
	pthread_t		id;
}	t_philo;

#endif