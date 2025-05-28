/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 14:33:28 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/28 18:21:19 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/philo.h"

void	*monitor(void *arg)
{
	t_philo	*club;
	int		i;

	club = (t_philo *)arg;
	while (1)
	{
		i = 0;
		while (i < club->num)
		{
			if (now_ms() - club->philos[i].meal_time > club->time_to_die + 1)
			{
				printf("%ld %d %s\n", now_ms() - club->start_time, i+1, DIED);
				club->died = 1;
				return (NULL);
			}
			i++;
		}
	}
	return (NULL);
}

void	*exist(void *arg)
{
	t_dude	*d;

	d = (t_dude *)arg;
	while (1)
	{
		printf("%ld %d %s\n", now_ms() - d->rules->start_time,
			d->philo_id, TNK);

		pthread_mutex_lock(&d->rules->butler);

		pthread_mutex_lock(&d->rules->forks[d->first_fork]);
		printf("%ld %d %s\n", now_ms() - d->rules->start_time,
			d->philo_id, FORK);
		pthread_mutex_lock(&d->rules->forks[d->second_fork]);
		printf("%ld %d %s\n", now_ms() - d->rules->start_time,
			d->philo_id, FORK);
		pthread_mutex_unlock(&d->rules->butler);
		printf("%ld %d %s\n", now_ms() - d->rules->start_time,
			d->philo_id, EAT);
		d->meal_time = now_ms();
		sleep_plus(now_ms() + d->rules->time_to_eat);
		pthread_mutex_unlock(&d->rules->forks[d->first_fork]);
		pthread_mutex_unlock(&d->rules->forks[d->second_fork]);
		printf("%ld %d %s\n", now_ms() - d->rules->start_time,
			d->philo_id, SLP);
		sleep_plus(now_ms() + d->rules->time_to_sleep);
		if (d->rules->died)
			return (NULL);
	}
	return (NULL);
}

void life_forks(t_philo *club, int start)
{
    int i = 0;
    if (start)
    {
        club->forks = malloc(sizeof(pthread_mutex_t) * club->num);
        while (i < club->num)
            pthread_mutex_init(&club->forks[i++], NULL);
        pthread_mutex_init(&club->butler, NULL);
    }
    else
    {
        i = 0;
        while (i < club->num)
            pthread_mutex_destroy(&club->forks[i++]);
        free(club->forks);
        pthread_mutex_destroy(&club->butler);
    }
}

static void init_philosophers(t_philo *club)
{
    t_dude *cur = club->philos;
    t_dude *end = cur + club->num;
    long   st  = club->start_time;

    while (cur < end)
    {
        size_t idx = cur - club->philos;
        cur->philo_id    = idx + 1;
        cur->meals_eaten = 0;
        cur->first_fork  = idx + (idx % 2 == 0);
        cur->second_fork = idx + (idx % 2 != 0);
        if (idx + 1 == club->num)
        {
            if (cur->second_fork == (int)(idx + 1))
                cur->second_fork = 0;
            else
                cur->first_fork = 0;
        }
        cur->meal_time = st;
        cur->rules     = club;
        pthread_mutex_init(&cur->meal_lock, NULL);
        cur++;
    }
}

static int launch_philosophers(t_philo *club)
{
    t_dude *cur = club->philos;
    t_dude *end = cur + club->num;
    int     started = 0;

    while (cur < end)
    {
        if (pthread_create(&cur->thread, NULL, exist, cur) != 0)
        {
            atomic_store(&club->died, 1);
            break;
        }
        started++;
        cur++;
    }
    return started;
}

int create_philosophers(t_philo *club)
{
    club->start_time = now_ms();
    init_philosophers(club);
    return launch_philosophers(club);
}

int	life(t_philo *club)
{
	int	i;
	int return = 1;
	club->philos = malloc(sizeof(t_dude) * club->num);
	life_forks(club, 1);
	i = create_philosophers(club);
	if (i - 1 == club->num)
		if (pthread_create(&club->m, NULL, monitor, club) != 0)
			atomic_store(&club->died, 1);
	else
		return = 0;
	while (--i > -1)
		pthread_join(club->philos[i].thread, NULL);
	pthread_join(club->m, NULL);
	free(club->philos);
	life_forks(club, 0);
	return (return);
}

int	init_club(t_philo *debate_club, char **av, int ac)
{
	debate_club->died = 0;
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
