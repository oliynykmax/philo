/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ai.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 14:33:28 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/28 16:10:01 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdatomic.h> // For _Atomic types
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define FORK "has taken a fork"
#define EAT "is eating"
#define DIED "died"
#define TNK "is thinking"
#define SLP "is sleeping"

# define INSTRUCTIONS \
	"accepted arguments:\n\
number_of_philosophers\n\
time_to_die\ntime_to_eat\ntime_to_sleep\n\
[number_of_times_each_philosopher_must_eat]\n"

typedef struct s_philo	t_philo;

typedef struct s_dude
{
	int					philo_id;
	int					first_fork;
	int					second_fork;
	int					meals_eaten;
	long				meal_time;
	pthread_t			thread;
	t_philo				*rules;
}						t_dude;

typedef struct s_philo
{
	int					num;
	int					time_to_die;
	int					time_to_eat;
	int					time_to_sleep;
	int					meals;
	long				start_time;
	_Atomic int all_eaten;
	_Atomic int died;
	pthread_mutex_t		butler;
	pthread_mutex_t		*forks;
	pthread_mutex_t		writing;
	pthread_t			m;
	t_dude				*philos;
}						t_philo;

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n)
	{
		if (s1[i] != s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		if (s1[i] == '\0' || s2[i] == '\0')
			break ;
		i++;
	}
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
	long	current;
	long	time_to_sleep;

	while (now_ms() < target_time)
	{
		if (atomic_load(&rules->died) || atomic_load(&rules->all_eaten))
			break ;
		current = now_ms();
		time_to_sleep = target_time - current;
		if (time_to_sleep <= 0)
			break ;
		if (time_to_sleep > 200)
			usleep(200);
		else
			usleep(time_to_sleep > 0 ? time_to_sleep : 0);
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
	else if (ft_strncmp(state, DIED, 4) == 0 && atomic_load(&d->rules->died))
	{
		current_time = now_ms() - d->rules->start_time;
		printf("%ld %d %s\n", current_time, d->philo_id, state);
	}
	pthread_mutex_unlock(&d->rules->writing);
}

void	*monitor(void *arg)
{
	t_philo	*club;
	int		i;
	int		philos_finished_eating;

	club = (t_philo *)arg;
	while (!atomic_load(&club->died) && !atomic_load(&club->all_eaten))
	{
		i = 0;
		philos_finished_eating = 0;
		while (i < club->num)
		{
			if (now_ms() - club->philos[i].meal_time > club->time_to_die + 1)
			{
				print_state(&club->philos[i], DIED);
				atomic_store(&club->died, 1);
				return (NULL);
			}
			if (club->meals > 0)
				if (club->philos[i].meals_eaten >= club->meals)
					philos_finished_eating++;
			i++;
		}
		if (club->meals > 0 && philos_finished_eating == club->num)
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
	if (d->philo_id % 2 == 0)
		usleep(d->rules->time_to_eat / 2 > 1 ? d->rules->time_to_eat / 2
			* 1000 : 1000);
	while (!atomic_load(&d->rules->died) && !atomic_load(&d->rules->all_eaten))
	{
		print_state(d, TNK);
		if (d->rules->num == 1)
		{
			pthread_mutex_lock(&d->rules->forks[d->first_fork]);
			print_state(d, FORK);
			sleep_plus(d->rules, now_ms() + d->rules->time_to_die + 100);
			pthread_mutex_unlock(&d->rules->forks[d->first_fork]);
			return (NULL);
		}
		pthread_mutex_lock(&d->rules->forks[d->first_fork]);
		print_state(d, FORK);
		pthread_mutex_lock(&d->rules->forks[d->second_fork]);
		print_state(d, FORK);
		print_state(d, EAT);
		d->meal_time = now_ms();
		sleep_plus(d->rules, now_ms() + d->rules->time_to_eat);
		d->meals_eaten++;
		pthread_mutex_unlock(&d->rules->forks[d->second_fork]);
		pthread_mutex_unlock(&d->rules->forks[d->first_fork]);
		if (atomic_load(&d->rules->died) || atomic_load(&d->rules->all_eaten))
			break ;
		if (d->rules->meals > 0 && d->meals_eaten >= d->rules->meals)
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
			pthread_mutex_destroy(&club->forks[i++]);
		free(club->forks);
		pthread_mutex_destroy(&club->writing);
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
		club->philos[i].meals_eaten = 0;
		club->philos[i].first_fork = i + (i % 2 == 0);
		club->philos[i].second_fork = i + (i % 2 != 0);
		if (i + 1 == club->num)
		{
			if (club->philos[i].second_fork == i + 1)
				club->philos[i].second_fork = 0;
			else
				club->philos[i].first_fork = 0;
		}
		club->philos[i].meal_time = club->start_time;
		club->philos[i].rules = club;
		if (pthread_create(&club->philos[i].thread, NULL, exist,
				&club->philos[i]) != 0)
		{
			atomic_store(&club->died, 1);
			return (i);
		}
		i++;
	}
	return (i);
}

int	life(t_philo *club)
{
	int	i;
	int	created_philos;

	club->philos = malloc(sizeof(t_dude) * club->num);
	if (!club->philos)
		return (0);
	life_forks(club, 1);
	created_philos = create_philosophers(club);
	if (created_philos < club->num)
	{
		i = 0;
		while (i < created_philos)
		{
			pthread_join(club->philos[i].thread, NULL);
			i++;
		}
		free(club->philos);
		life_forks(club, 0);
		return (0);
	}
	if (pthread_create(&club->m, NULL, monitor, club) != 0)
	{
		atomic_store(&club->died, 1);
		i = 0;
		while (i < club->num)
		{
			pthread_join(club->philos[i].thread, NULL);
			i++;
		}
		free(club->philos);
		life_forks(club, 0);
		return (0);
	}
	i = 0;
	while (i < club->num)
	{
		pthread_join(club->philos[i].thread, NULL);
		i++;
	}
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
	{
		printf("Error: Simulation failed to run.\n");
		return (EXIT_FAILURE);
	}
	return (0);
}
