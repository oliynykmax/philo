/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 14:33:28 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/28 12:33:08 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/philo.h"

void	*monitor(void *arg)
{
	t_philo	*club;
	int		i;

	i = 0;
	club = (t_philo *)arg;
	(void)club;
	(void)i;
	/* 	while (1)
		{
			if (club->cool[i].state == -1)
			{
				printf("%ld %d %s\n", club->start, club->cool[i].philo_id,
					DIED);
				club->end = 1;
				break ;
			}
			if (i + 1 == club->id)
				i = 0;
			else
				i++;
		} */
	return (NULL);
}

void	sleep_plus(long target_time)
{
	long	current;
	long	time_to_sleep;

	current = now_ms();
	time_to_sleep = target_time - current;
	if (time_to_sleep <= 0)
		return ;
	if (time_to_sleep > 1)
		usleep((time_to_sleep - 1) * 1000);
	while (now_ms() < target_time)
		usleep(50);
}

void	*exist(void *arg)
{
	t_dude	*dude;

	dude = (t_dude *)arg;
	while (1)
	{
		printf("%ld %d %s\n", now_ms() - dude->rules->start_time,
			dude->philo_id, TNK);
		pthread_mutex_lock(&dude->rules->forks[dude->first_fork]);
		printf("%ld %d %s\n", now_ms() - dude->rules->start_time,
			dude->philo_id, FORK);
		pthread_mutex_lock(&dude->rules->forks[dude->second_fork]);
		printf("%ld %d %s\n", now_ms() - dude->rules->start_time,
			dude->philo_id, FORK);
		printf("%ld %d %s\n", now_ms() - dude->rules->start_time,
			dude->philo_id, EAT);
		sleep_plus(now_ms() + dude->rules->time_to_eat);
		pthread_mutex_unlock(&dude->rules->forks[dude->first_fork]);
		pthread_mutex_unlock(&dude->rules->forks[dude->second_fork]);
		printf("%ld %d %s\n", now_ms() - dude->rules->start_time,
			dude->philo_id, SLP);
		sleep_plus(now_ms() + dude->rules->time_to_sleep);
	}
	return (NULL);
}

void	life_forks(t_philo *club, int start)
{
	int	i;

	i = 0;
	if (start)
	{
		club->forks = malloc(sizeof(pthread_mutex_t) * club->num);
		if (!club->forks)
			return ;
		while (i < club->num)
			pthread_mutex_init(&club->forks[i++], NULL);
	}
	else
	{
		while (i < club->num)
			pthread_mutex_destroy(&club->forks[i++]);
		free(club->forks);
		club->forks = NULL;
	}
}

int	create_philosophers(t_philo *club)
{
	int	i;

	i = 0;
	club->start_time = now_ms();
	while (i < club->num)
	{
		club->philos[i].philo_id = i + 1;
		club->philos[i].first_fork = i + (int)(i % 2 == 0);
		club->philos[i].second_fork = i + (int)(i % 2 != 0);
		if (i + 1 == club->num)
		{
			if (club->philos[i].second_fork == i + 1)
				club->philos[i].second_fork = 0;
			else
				club->philos[i].first_fork = 0;
		}
		club->philos[i].rules = club;
		if (pthread_create(&club->philos[i].thread, NULL, exist,
				&club->philos[i]) != 0)
			return (i);
		i++;
	}
	return (i);
}

int	life(t_philo *club)
{
	int	i;

	club->philos = malloc(sizeof(t_dude) * club->num);
	life_forks(club, 1);
	i = create_philosophers(club);
	pthread_create(&club->m, NULL, monitor, club);
	while (--i > -1)
		pthread_join(club->philos[i].thread, NULL);
	pthread_join(club->m, NULL);
	free(club->philos);
	life_forks(club, 0);
	return (1);
}

int	init_club(t_philo *debate_club, char **av, int ac)
{
	debate_club->num = ft_parser(av[1]);
	debate_club->time_to_die = ft_parser(av[2]);
	debate_club->time_to_eat = ft_parser(av[3]);
	debate_club->time_to_sleep = ft_parser(av[4]);
	if (debate_club->num < 1 || debate_club->time_to_sleep < 1
		|| debate_club->time_to_eat < 1 || debate_club->time_to_sleep < 1)
		return (-1);
	if (ac == 6)
	{
		debate_club->meals = ft_parser(av[5]);
		if (debate_club->meals < 1)
			return (-1);
	}
	return (0);
}

int	main(int ac, char **av)
{
	t_philo	debate_club;

	if (ac != 5 && ac != 6)
	{
		printf(INSTRUCTIONS);
		return (0);
	}
	if (init_club(&debate_club, av, ac) != 0)
		return (EXIT_FAILURE);
	if (life(&debate_club) == 0)
		return (EXIT_FAILURE);
	return (0);
}
