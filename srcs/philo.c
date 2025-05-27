/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 14:33:28 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/27 20:01:54 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/philo.h"
#include <pthread.h>

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
void	*exist(void *arg)
{
	t_dude	*dude;

	dude = (t_dude *)arg;
	if (dude->philo_id == 1)
		dude->rules->start_time = current_time_ms();
	printf("my name: %d, my left fork: %d, my right fork: %d\n", dude->philo_id, dude->left_fork, dude->right_fork);
	while (1)
	{
		printf("%ld %d %s\n", current_time_ms() - dude->rules->start_time, dude->philo_id, TNK);
		pthread_mutex_lock(&dude->rules->forks[dude->left_fork]);
		printf("%ld %d %s\n", current_time_ms() - dude->rules->start_time, dude->philo_id, FORK);
		pthread_mutex_lock(&dude->rules->forks[dude->right_fork]);
		printf("%ld %d %s\n", current_time_ms() - dude->rules->start_time, dude->philo_id, FORK);
		printf("%ld %d %s\n", current_time_ms() - dude->rules->start_time, dude->philo_id, EAT);
		usleep(1000 * dude->rules->time_to_eat);
		pthread_mutex_unlock(&dude->rules->forks[dude->left_fork]);
		pthread_mutex_unlock(&dude->rules->forks[dude->right_fork]);
		printf("%ld %d %s\n", current_time_ms() - dude->rules->start_time, dude->philo_id, SLP);
		usleep(900 * dude->rules->time_to_sleep);
	}
	return (NULL);
}
/* void	*exist(void *arg)
{
	t_dude	*club;
	long	elapsed;
	int		i;

	club = (t_philo *)arg;
	club->cool->philo_id = atomic_fetch_add(&club->id, 1);
	if (club->cool->philo_id == 1)
		club->start = current_time_ms();
	printf("0 %d is thinking\n", club->cool->philo_id);
	usleep(1000 * club->time_to_eat);
	while (1)
	{
		elapsed = current_time_ms() - club->start;
		if (club->end)
			return (NULL);
		printf("%ld %d %s\n", elapsed, club->cool->philo_id, EAT);
		usleep(1000 * (club->time_to_eat));
		if (elapsed > club->time_to_die)
			club->cool->state = -1;
	}
	return (NULL);
} */
void	life_forks(t_philo *club, int start)
{
	int i = 0;
	if (start)
	{
		club->forks = malloc(sizeof(pthread_mutex_t) * club->num);
		if (!club->forks)
			return ;
		while (i < club->num)
		{
			pthread_mutex_init(&club->forks[i], NULL);
			i++;
		}
	}
	else
	{
		while (i < club->num)
		{
			pthread_mutex_destroy(&club->forks[i]);
			i++;
		}
		free(club->forks);
		club->forks = NULL;
	}
}

int	create_philosophers(t_philo *club)
{
	int	i;

	i = 0;
	while (i < club->num)
	{
		club->philos[i].philo_id = i + 1;
		club->philos[i].left_fork = i + (int)(i % 2 == 0);
		club->philos[i].right_fork = i + (int)(i % 2 != 0);
		if (i + 1 == club->num)
		{
			if (club->philos[i].right_fork == i+1)
				club->philos[i].right_fork = 0;
			else
				club->philos[i].left_fork = 0;
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

/* int	life(t_philo *club)
{
	int	i;

	club->group = malloc(sizeof(pthread_t) * club->num);
	club->cool = malloc(sizeof(t_dude) * club->num);
	i = 0;
	while (i < club->num && pthread_create(&club->group[i], NULL, exist,
			club) == 0)
		i++;
	pthread_create(&club->m, NULL, monitor, club);
	while (--i > -1)
		pthread_join(club->group[i], NULL);
	free(club->group);
	free(club->cool);
	return (1);
} */
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
