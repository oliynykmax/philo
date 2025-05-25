/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 14:33:28 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/25 14:22:26 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/philo.h"
#include <stdlib.h>

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
	return (num);
}

long	current_time_ms(void)
{
	struct timeval	t;

	gettimeofday(&t, NULL);
	return (t.tv_sec * 1000L + t.tv_usec / 1000);
}

// void	*monitor(void *arg)
// {
// }

void	*exist(void *arg)
{
	t_philo		*club;
	static int	id_ctr = 0;
	int			philo_id;
	long		elapsed;

	club = (t_philo *)arg;
	pthread_mutex_lock(&club->lock);
	philo_id = ++id_ctr;
	pthread_mutex_unlock(&club->lock);
	pthread_barrier_wait(&club->barrier);
	if (philo_id == 1)
		club->start = current_time_ms();
	pthread_barrier_wait(&club->barrier);
	printf("0 philo %d started thinking\n", philo_id);
	usleep(1000 * club->time_to_eat);
	while (1)
	{
		elapsed = current_time_ms() - club->start;
		if (elapsed >= club->time_to_die)
		{
			printf("%ld philo %d %s\n", elapsed, philo_id, DIED);
			break ;
		}
		printf("%ld philo %d %s\n", elapsed, philo_id, EAT);
		usleep(1000 * club->time_to_eat);
	}
	return (NULL);
}

// void	*wake_up(void *arg)
// {
// 	t_philo		*club;
// 	long		time_passed;
// 	long		target_time;
// 	int			philo_id;
// 	static int	id = 0;
//
// 	club = (t_philo *)arg;
// 	pthread_mutex_lock(&club->lock);
// 	philo_id = ++id;
// 	pthread_mutex_unlock(&club->lock);
// 	pthread_barrier_wait(&club->barrier);
// 	if (philo_id == 1)
// 	{
// 		pthread_mutex_lock(&club->lock);
// 		club->start = current_time_ms();
// 		pthread_mutex_unlock(&club->lock);
// 	}
// 	pthread_barrier_wait(&club->barrier);
// 	while (1)
// 	{
// 		pthread_mutex_lock(&club->lock);
// 		target_time = club->start + club->time_to_eat;
// 		time_passed = current_time_ms();
// 		if (time_passed >= target_time)
// 		{
// 			printf("%d %ld\n", philo_id, time_passed - club->start);
// 			pthread_mutex_unlock(&club->lock);
// 			break ;
// 		}
// 		pthread_mutex_unlock(&club->lock);
// 		usleep(100);
// 	}
// 	return (NULL);
// }
void	life(t_philo *data)
{
	int	i;

	data->group = malloc(sizeof(pthread_t) * data->id);
	pthread_mutex_init(&data->lock, NULL);
	pthread_barrier_init(&data->barrier, NULL, data->id);
	i = 0;
	while (i < data->id)
	{
		if (pthread_create(&data->group[i], NULL, exist, data) != 0)
		{
			printf("Thread creation failed\n");
			break ;
		}
		i++;
	}
	i = 0;
	while (i < data->id)
	{
		pthread_join(data->group[i], NULL);
		i++;
	}
	free(data->group);
	pthread_mutex_destroy(&data->lock);
	pthread_barrier_destroy(&data->barrier);
}

int	main(int ac, char **av)
{
	t_philo	debate_club;

	if (ac != 5 && ac != 6)
	{
		printf(INSTRUCTIONS);
		return (0);
	}
	debate_club.id = ft_parser(av[1]);
	debate_club.time_to_die = ft_parser(av[2]);
	debate_club.time_to_eat = ft_parser(av[3]);
	debate_club.time_to_sleep = ft_parser(av[4]);
	if (debate_club.id < 1 || debate_club.time_to_sleep < 1
		|| debate_club.time_to_eat < 1 || debate_club.time_to_sleep < 1)
		return (EXIT_FAILURE);
	if (ac == 6)
		debate_club.meals = ft_parser(av[5]);
	life(&debate_club);
}
