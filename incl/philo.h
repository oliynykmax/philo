/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 14:30:12 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/29 14:50:24 by maoliiny         ###   ########.fr       */
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

typedef struct s_philo	t_philo;

typedef struct s_dude
{
	int					philo_id;
	int					first_fork;
	int					second_fork;
	int					meals_eaten;
	long				meal_time;
	pthread_t			thread;
	t_philo				*r;
	pthread_mutex_t		meal_lock;
	int					hunger;
}						t_dude;

typedef struct s_philo
{
	int					num;
	int					time_to_die;
	int					time_to_eat;
	int					time_to_sleep;
	int					meals;
	long				start_time;
	atomic_int			all_eaten;
	atomic_int			died;
	pthread_mutex_t		*forks;
	pthread_mutex_t		writing;
	pthread_t			m;
	t_dude				*philos;
}						t_philo;

long					now_ms(void);
long					ft_parser(const char *nptr);
void					sleep_plus(t_philo *r, long target_time);
void					print_state(t_dude *d, const char *state);
void					*exist(void *arg);
int						life_forks(t_philo *club, int start);
void					*monitor(void *arg);

#endif
