/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 15:33:45 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/29 14:24:49 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/philo.h"

int	life_forks(t_philo *club, int start)
{
	int	i;

	i = 0;
	if (start)
	{
		club->forks = malloc(sizeof(pthread_mutex_t) * club->num);
		if (!club->forks)
			return (1);
		while (i < club->num)
			pthread_mutex_init(&club->forks[i++], NULL);
		pthread_mutex_init(&club->writing, NULL);
	}
	else
	{
		i = 0;
		while (i < club->num)
		{
			pthread_mutex_destroy(&club->forks[i]);
			pthread_mutex_destroy(&club->philos[i].meal_lock);
			i++;
		}
		free(club->forks);
		pthread_mutex_destroy(&club->writing);
	}
	return (0);
}

long	now_ms(void)
{
	struct timeval	t;

	gettimeofday(&t, NULL);
	return (t.tv_sec * 1000L + t.tv_usec / 1000);
}

long	ft_parser(const char *nptr)
{
	long	num;
	int		dig;

	num = 0;
	while (*nptr >= '0' && *nptr <= '9')
	{
		dig = *nptr - '0';
		num = num * 10 + dig;
		nptr++;
	}
	if (*nptr != '\0')
		return (0);
	return (num);
}

void	sleep_plus(t_philo *r, long target_time)
{
	long	current_ms;
	long	time_to_wait_us;
	long	sleep_chunk;

	current_ms = now_ms();
	while (current_ms < target_time)
	{
		if (atomic_load(&r->died) || atomic_load(&r->all_eaten))
			break ;
		time_to_wait_us = (target_time - current_ms) * 1000;
		if (time_to_wait_us <= 0)
			break ;
		if (time_to_wait_us > 200)
			sleep_chunk = 200;
		else
			sleep_chunk = time_to_wait_us;
		usleep(sleep_chunk);
		current_ms = now_ms();
	}
}

void	print_state(t_dude *d, const char *state)
{
	long	current_time;

	pthread_mutex_lock(&d->r->writing);
	if (!atomic_load(&d->r->died) && !atomic_load(&d->r->all_eaten))
	{
		current_time = now_ms() - d->r->start_time;
		printf("%ld %d %s\n", current_time, d->philo_id, state);
	}
	else if (state[0] == 'd' && atomic_load(&d->r->died))
	{
		current_time = now_ms() - d->r->start_time;
		printf("%ld %d %s\n", current_time, d->philo_id, state);
	}
	pthread_mutex_unlock(&d->r->writing);
}
