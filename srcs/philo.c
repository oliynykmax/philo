/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 14:33:28 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/23 18:46:45 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/philo.h"

long ft_atoi(const char *nptr) {
  long num;
  long minus;
  int dig;

  num = 0;
  minus = 1;
  while ((*nptr >= 9 && *nptr <= 13) || *nptr == 32)
    nptr++;
  if (*nptr == '+' || *nptr == '-')
    if (*nptr++ == '-')
      minus = -1;
  while (*nptr >= '0' && *nptr <= '9') {
    dig = *nptr - '0';
    num = num * 10 + dig;
    nptr++;
  }
  return (num * minus);
}

long current_time_ms(void) {
  struct timeval t;

  gettimeofday(&t, NULL);
  return (t.tv_sec * 1000L + t.tv_usec / 1000);
}

void *wake_up(void *arg) {
  t_debate *club;
  long time_passed;
  long target_time;
  int philo_id;
  static int id = 0;

  club = (t_debate *)arg;
  // Assign unique ID to each philosopher
  pthread_mutex_lock(&club->lock);
  philo_id = ++id;
  pthread_mutex_unlock(&club->lock);
  // Wait for all threads to be ready
  pthread_barrier_wait(&club->barrier);
  // If this is the first philosopher, set the start time
  if (philo_id == 1) {
    pthread_mutex_lock(&club->lock);
    club->start = current_time_ms();
    pthread_mutex_unlock(&club->lock);
  }
  // Wait again to ensure all have the same start time
  pthread_barrier_wait(&club->barrier);
  while (1) {
    pthread_mutex_lock(&club->lock);
    target_time = club->start + club->time_to_eat;
    time_passed = current_time_ms();
    if (time_passed >= target_time) {
      printf("%d %ld\n", philo_id, time_passed - club->start);
      pthread_mutex_unlock(&club->lock);
      break;
    }
    pthread_mutex_unlock(&club->lock);
    usleep(100);
  }
  return (NULL);
}

void life(t_philo *data) {
  t_debate club;
  int i;

  club.time_to_eat = data->time_to_eat;
  club.philo_count = data->id;
  club.group = malloc(sizeof(pthread_t) * data->id);
  // Initialize synchronization primitives
  pthread_mutex_init(&club.lock, NULL);
  pthread_barrier_init(&club.barrier, NULL, data->id);
  i = 0;
  while (i < data->id) {
    if (pthread_create(&club.group[i], NULL, wake_up, &club) != 0) {
      // Handle error
      printf("Thread creation failed\n");
      break;
    }
    i++;
  }
  i = 0;
  while (i < data->id) {
    pthread_join(club.group[i], NULL);
    i++;
  }
  // Cleanup
  free(club.group);
  pthread_mutex_destroy(&club.lock);
  pthread_barrier_destroy(&club.barrier);
}

int main(int ac, char **av) {
  t_philo debate_club;

  if (ac != 5 && ac != 6) {
    printf(INSTRUCTIONS);
    return (0);
  }
  debate_club.id = ft_atoi(av[1]);
  debate_club.time_to_die = ft_atoi(av[2]);
  debate_club.time_to_eat = ft_atoi(av[3]);
  debate_club.time_to_sleep = ft_atoi(av[4]);
  if (ac == 6)
    debate_club.meals = ft_atoi(av[5]);
  life(&debate_club);
}
