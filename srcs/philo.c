/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 14:33:28 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/25 22:20:37 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/philo.h"

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

long	current_time_ms(void)
{
	struct timeval	t;

	gettimeofday(&t, NULL);
	return (t.tv_sec * 1000L + t.tv_usec / 1000);
}

void	*monitor(void *arg)
{
	t_philo	*club;
	int		i;

	i = 0;
	club = (t_philo *)arg;
	while (1)
	{
		if (club->cool[i].state == -1)
		{
			printf("%ld %d %s\n", club->start, club->cool[i].philo_id, DIED);
			club->end = 1;
			break ;
		}
		if (i + 1 == club->id)
			i = 0;
		else
			i++;
	}
	return (NULL);
}

void	*exist(void *arg)
{
	t_philo	*club;
	long	elapsed;

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
}

int	life(t_philo *club)
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
}

int	init_club(t_philo *debate_club, char **av, int ac)
{
	debate_club->num = ft_parser(av[1]);
	debate_club->forks = debate_club->num;
	debate_club->time_to_die = ft_parser(av[2]);
	debate_club->time_to_eat = ft_parser(av[3]);
	debate_club->time_to_sleep = ft_parser(av[4]);
	debate_club->id = 1;
	debate_club->end = 0;
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
