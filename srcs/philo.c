/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 11:55:34 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/29 13:19:53 by maoliiny         ###   ########.fr       */
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
}

static void	init_philosophers(t_philo *club)
{
	int	idx;

	idx = 0;
	while (idx < club->num)
	{
		club->philos[idx].philo_id = idx + 1;
		club->philos[idx].meals_eaten = 0;
		club->philos[idx].meal_time = club->start_time;
		club->philos[idx].rules = club;
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
	life_forks(club, 1);
	club->start_time = now_ms();
	init_philosophers(club);
	i = launch_philosophers(club);
	if (pthread_create(&club->m, NULL, monitor, club) != 0)
		atomic_store(&club->died, 1);
	while (--i > -1)
		pthread_join(club->philos[i].thread, NULL);
	pthread_join(club->m, NULL);
	free(club->philos);
	life_forks(club, 0);
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
