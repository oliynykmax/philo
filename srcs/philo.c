/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 11:55:34 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/29 14:24:31 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/philo.h"

static void	init_philosophers(t_philo *club)
{
	int	idx;

	idx = 0;
	while (idx < club->num)
	{
		club->philos[idx].philo_id = idx + 1;
		club->philos[idx].meals_eaten = 0;
		club->philos[idx].meal_time = club->start_time;
		club->philos[idx].r = club;
		if (idx % 2 == 0)
		{
			club->philos[idx].first_fork = (idx + 1) % club->num;
			club->philos[idx].second_fork = idx;
		}
		else
		{
			club->philos[idx].first_fork = idx;
			club->philos[idx].second_fork = (idx + 1) % club->num;
		}
		pthread_mutex_init(&club->philos[idx].meal_lock, NULL);
		idx++;
	}
}

static int	launch_philosophers(t_philo *club)
{
	t_dude	*cur;
	t_dude	*end;
	int		started;

	cur = club->philos;
	end = cur + club->num;
	started = 0;
	while (cur < end)
	{
		if (pthread_create(&cur->thread, NULL, exist, cur) != 0)
		{
			atomic_store(&club->died, 1);
			break ;
		}
		started++;
		cur++;
	}
	return (started);
}

int	life(t_philo *club)
{
	int	i;

	club->philos = malloc(sizeof(t_dude) * club->num);
	if (!club->philos)
		return (0);
	if (life_forks(club, 1))
	{
		free(club->philos);
		return (0);
	}
	club->start_time = now_ms();
	init_philosophers(club);
	i = launch_philosophers(club);
	if (pthread_create(&club->m, NULL, monitor, club) != 0)
		atomic_store(&club->died, 1);
	while (--i > -1)
		pthread_join(club->philos[i].thread, NULL);
	pthread_join(club->m, NULL);
	life_forks(club, 0);
	free(club->philos);
	return (1);
}

int	init_club(t_philo *debate_club, char **av, int ac)
{
	atomic_init(&debate_club->died, 0);
	atomic_init(&debate_club->all_eaten, 0);
	debate_club->meals = 0;
	debate_club->num = ft_parser(av[1]);
	debate_club->time_to_die = ft_parser(av[2]);
	debate_club->time_to_eat = ft_parser(av[3]);
	debate_club->time_to_sleep = ft_parser(av[4]);
	if (debate_club->num < 1 || debate_club->time_to_die < 1
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

	memset(&debate_club, 0, sizeof(t_philo));
	if (ac != 5 && ac != 6)
	{
		printf(INSTRUCTIONS);
		return (0);
	}
	if (init_club(&debate_club, av, ac) != 0)
	{
		printf("Error: Invalid arguments.\n");
		return (EXIT_FAILURE);
	}
	if (life(&debate_club) == 0)
		return (EXIT_FAILURE);
	return (0);
}
