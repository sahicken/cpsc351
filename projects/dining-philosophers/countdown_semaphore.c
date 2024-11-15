#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5

sem_t chopsticks[NUM_PHILOSOPHERS];

void think(int id) {
    printf("Philosopher %d is thinking...\n", id);
    sleep(rand() % 3);
}

void eat(int id) {
    printf("Philosopher %d is eating...\n", id);
    sleep(rand() % 3);
}

void* philosopher(void* arg) {
    int id = *((int*)arg);
    int left = id;
    int right = (id + 1) % NUM_PHILOSOPHERS;

    while (1) {
        think(id);

        // Pick up chopsticks
        if (id % 2 == 0) {
            sem_wait(&chopsticks[left]);
            //printf("Philosopher %d picked up left chopstick %d\n", id, left);
            sem_wait(&chopsticks[right]);
            //printf("Philosopher %d picked up right chopstick %d\n", id, right);
        } else {
            sem_wait(&chopsticks[right]);
            //printf("Philosopher %d picked up right chopstick %d\n", id, right);
            sem_wait(&chopsticks[left]);
            //printf("Philosopher %d picked up left chopstick %d\n", id, left);
        }

        eat(id);

        // Put down chopsticks
        sem_post(&chopsticks[left]);
        //printf("Philosopher %d put down left chopstick %d\n", id, left);
        sem_post(&chopsticks[right]);
        //printf("Philosopher %d put down right chopstick %d\n", id, right);
    }

    return NULL;
}

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];
    int ids[NUM_PHILOSOPHERS];

    // Initialize the semaphores (chopsticks)
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_init(&chopsticks[i], 0, 1);
    }

    // Create philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &ids[i]);
    }

    // Wait for all philosophers (this will never happen in this implementation)
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    // Destroy the semaphores
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_destroy(&chopsticks[i]);
    }

    return 0;
}