# 🧠 Philosophers

An implementation of the classic Dining Philosophers problem, developed as part of the 42 curriculum. This project delves into the fundamentals of multithreading, mutexes, and process synchronization.

![Project Score](./score.png)

## 📄 Table of Contents

- [Introduction](#introduction)
- [Project Structure](#project-structure)
- [Usage](#usage)
- [Testing](#testing)
- [Resources](#resources)

## 📘 Introduction

The Dining Philosophers problem illustrates synchronization issues and techniques in concurrent programming. In this project, each philosopher is a thread that alternates between thinking, eating, and sleeping. To eat, a philosopher must acquire two forks (mutexes), ensuring proper synchronization to avoid deadlocks and starvation.

## 🗂 Project Structure

- `philo/`: Contains the mandatory part of the project using threads and mutexes.
- `tester/`: Includes an enhanced version of Jules' tester for automated testing.

## 🛠 Usage

### Compilation

Navigate to the `philo/` directory and run:

```bash
make
```

### Execution

```bash
./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```

**Parameters:**

- `number_of_philosophers`: Total number of philosophers and forks.
- `time_to_die`: Time (in ms) a philosopher can go without eating before dying.
- `time_to_eat`: Time (in ms) it takes for a philosopher to eat.
- `time_to_sleep`: Time (in ms) a philosopher spends sleeping.
- `number_of_times_each_philosopher_must_eat` (optional): Simulation ends when each philosopher has eaten this many times.

**Example:**

```bash
./philo 5 800 200 200 3
```

## ✅ Testing

A hardcore version of Jules' tester is included in the `tester/` directory. This tester automates various test cases to validate the correctness and robustness of your implementation.

**Usage:**

```bash
cd tester
cp ../philo .
bash test.sh
```

The tester will run multiple scenarios and provide feedback on the program's behavior under different conditions.

## 📚 Resources

- [Jules' Original Tester](https://github.com/Jules478/philosophers_Tester/)

---

*This project was developed as part of the 42 curriculum to deepen understanding of concurrency, synchronization, and process management in C.*

