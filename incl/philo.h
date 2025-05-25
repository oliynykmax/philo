/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 14:30:12 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/25 11:19:41 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

# define INSTRUCTIONS                         \
	"accepted arguments:\n"                  \
	"number_of_philosophers"                 \
	" time_to_die time_to_eat time_to_sleep" \
	" [number_of_times_each_philosopher_must_eat]"

typedef struct t_philo
{
	int					id;
	long				time_to_die;
	long				time_to_eat;
	long				time_to_sleep;
	int					meals;
	long				start_time;
}						t_philo;

typedef struct s_debate
{
	pthread_mutex_t		lock;
	pthread_t			*group;
	long				start;
	long				time_to_eat;
	int					philo_count;
	int					*ready;
	pthread_barrier_t	barrier;
}						t_debate;

#endif
