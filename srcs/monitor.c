/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 13:57:05 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/29 14:51:37 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/philo.h"

static int	philo_timed_out(t_philo *club, int idx)
{
	t_dude	*d;
	long	last;

	d = &club->philos[idx];
	pthread_mutex_lock(&d->meal_lock);
	last = d->meal_time;
	pthread_mutex_unlock(&d->meal_lock);
	return (now_ms() - last > club->time_to_die + 1);
}

static int	scan_philosophers(t_philo *club, int *finished_count)
{
	int	i;

	i = 0;
	while (i < club->num)
	{
		if (philo_timed_out(club, i))
		{
			atomic_store(&club->died, 1);
			print_state(&club->philos[i], DIED);
			return (1);
		}
		if (club->meals > 0 && club->philos[i].meals_eaten >= club->meals)
			(*finished_count)++;
		i++;
	}
	return (0);
}

void	*monitor(void *arg)
{
	t_philo	*club;
	int		finished;

	club = (t_philo *)arg;
	while (!atomic_load(&club->died) && !atomic_load(&club->all_eaten))
	{
		finished = 0;
		if (scan_philosophers(club, &finished))
			return (NULL);
		if (club->meals > 0 && finished == club->num)
		{
			atomic_store(&club->all_eaten, 1);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
