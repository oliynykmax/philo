/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exist.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 13:16:36 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/30 21:02:22 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/philo.h"

static void	handle_single_philo(t_dude *d)
{
	print_state(d, TNK);
	pthread_mutex_lock(&d->r->forks[d->first_fork]);
	print_state(d, FORK);
	sleep_plus(d->r, now_ms() + d->r->time_to_die + 50);
	pthread_mutex_unlock(&d->r->forks[d->first_fork]);
}

static void	grab_forks(t_dude *d)
{
	pthread_mutex_lock(&d->r->forks[d->first_fork]);
	print_state(d, FORK);
	pthread_mutex_lock(&d->r->forks[d->second_fork]);
	print_state(d, FORK);
}

static void	eat_phase(t_dude *d)
{
	print_state(d, EAT);
	pthread_mutex_lock(&d->meal_lock);
	d->meal_time = now_ms();
	pthread_mutex_unlock(&d->meal_lock);
	sleep_plus(d->r, now_ms() + d->r->time_to_eat);
	d->meals_eaten += 1;
}

static void	sleep_phase(t_dude *d)
{
	print_state(d, SLP);
	sleep_plus(d->r, now_ms() + d->r->time_to_sleep);
}

void	*exist(void *arg)
{
	t_dude	*d;

	d = (t_dude *)arg;
	if (d->r->num % 2 == 0 && d->philo_id % 2)
		usleep(1000);
	if (d->r->num == 1)
		handle_single_philo(d);
	while (!d->r->died && !d->r->all_eaten)
	{
		print_state(d, TNK);
		if (now_ms() - d->meal_time > d->r->time_to_sleep && d->r->num % 2 == 1
			&& d->r->time_to_eat < d->r->time_to_sleep)
			usleep(750);
		else if (now_ms() - d->meal_time > d->r->time_to_sleep && d->r->num
			% 2 == 1)
			sleep_plus(d->r, now_ms() - 2 + d->r->time_to_eat);
		grab_forks(d);
		eat_phase(d);
		pthread_mutex_unlock(&d->r->forks[d->second_fork]);
		pthread_mutex_unlock(&d->r->forks[d->first_fork]);
		if (!d->r->died && !d->r->all_eaten)
			sleep_phase(d);
	}
	return (NULL);
}
