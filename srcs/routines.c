/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routines.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 13:16:36 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/29 13:16:37 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/philo.h"

void	*monitor(void *arg)
{
	t_philo	*club;
	int		i;
	int		philos_finished_eating_this_cycle;
	long	current_philo_meal_time;

	club = (t_philo *)arg;
	while (!atomic_load(&club->died) && !atomic_load(&club->all_eaten))
	{
		i = 0;
		philos_finished_eating_this_cycle = 0;
		while (i < club->num)
		{
			pthread_mutex_lock(&club->philos[i].meal_lock);
			current_philo_meal_time = club->philos[i].meal_time;
			pthread_mutex_unlock(&club->philos[i].meal_lock);
			if (now_ms() - current_philo_meal_time > club->time_to_die)
			{
				atomic_store(&club->died, 1);
				print_state(&club->philos[i], DIED);
				return (NULL);
			}
			if (club->meals > 0)
				if (club->philos[i].meals_eaten >= club->meals)
					philos_finished_eating_this_cycle++;
			i++;
		}
		if (club->meals > 0 && philos_finished_eating_this_cycle == club->num)
		{
			atomic_store(&club->all_eaten, 1);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}

void	*exist(void *arg)
{
	t_dude	*d;

	d = (t_dude *)arg;
	if (d->philo_id % 2)
		usleep(500);
	while (!atomic_load(&d->rules->died) && !atomic_load(&d->rules->all_eaten))
	{
		print_state(d, TNK);
		if (d->rules->num == 1)
		{
			pthread_mutex_lock(&d->rules->forks[d->first_fork]);
			print_state(d, FORK);
			sleep_plus(d->rules, now_ms() + d->rules->time_to_die + 50);
			pthread_mutex_unlock(&d->rules->forks[d->first_fork]);
			return (NULL);
		}
		if (now_ms() - d->meal_time > d->rules->time_to_sleep && d->rules->num
			% 2 == 1)
			usleep(500);
		pthread_mutex_lock(&d->rules->forks[d->first_fork]);
		print_state(d, FORK);
		pthread_mutex_lock(&d->rules->forks[d->second_fork]);
		print_state(d, FORK);
		print_state(d, EAT);
		pthread_mutex_lock(&d->meal_lock);
		d->meal_time = now_ms();
		pthread_mutex_unlock(&d->meal_lock);
		sleep_plus(d->rules, now_ms() + d->rules->time_to_eat);
		d->meals_eaten++;
		pthread_mutex_unlock(&d->rules->forks[d->second_fork]);
		pthread_mutex_unlock(&d->rules->forks[d->first_fork]);
		if (atomic_load(&d->rules->died) || atomic_load(&d->rules->all_eaten))
			break ;

		print_state(d, SLP);
		sleep_plus(d->rules, now_ms() + d->rules->time_to_sleep);
	}
	return (NULL);
}
