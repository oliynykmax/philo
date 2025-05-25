/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 14:30:12 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/25 22:06:48 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdatomic.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>
# define FORK "has taken a fork"
# define EAT "is eating"
# define DIED "died"
# define TNK "is thinking"
# define SLP "is sleeping"
# define INSTRUCTIONS \
	"accepted arguments:\n\
number_of_philosophers\n\
time_to_die\ntime_to_eat\ntime_to_sleep\n\
[number_of_times_each_philosopher_must_eat]\n"

typedef struct dude
{
	long				meal_time;
	int					philo_id;
	int					state;
}						t_dude;

typedef struct t_philo
{
	int					num;
	long				time_to_die;
	long				time_to_eat;
	long				time_to_sleep;
	int					meals;
	int					forks;
	pthread_mutex_t		lock;
	pthread_t			*group;
	pthread_t			m;
	long				start;
	pthread_barrier_t	barrier;
	atomic_int			id;
	t_dude				*cool;
	int					end;
}						t_philo;

#endif
