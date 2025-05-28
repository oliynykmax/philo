/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 15:33:45 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/28 15:20:56 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/philo.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	if (i == n)
		return (0);
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
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

void	sleep_plus(t_philo *rules, long target_time)
{
	long	current_ms;
	long	time_to_wait_us;
	long	sleep_chunk;

	current_ms = now_ms();
	while (current_ms < target_time)
	{
		if (atomic_load(&rules->died) || atomic_load(&rules->all_eaten))
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

	pthread_mutex_lock(&d->rules->writing);
	if (!atomic_load(&d->rules->died) && !atomic_load(&d->rules->all_eaten))
	{
		current_time = now_ms() - d->rules->start_time;
		printf("%ld %d %s\n", current_time, d->philo_id, state);
	}
	else if (ft_strncmp(state, DIED, 4) == 0
		&& atomic_load(&d->rules->died))
	{
		current_time = now_ms() - d->rules->start_time;
		printf("%ld %d %s\n", current_time, d->philo_id, state);
	}
	pthread_mutex_unlock(&d->rules->writing);
}
