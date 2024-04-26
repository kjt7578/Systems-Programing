//0403
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 5

typedef int data_t;

typedef struct {
    data_t buffer[BUFFER_SIZE];
    int in;
    int out;
    sem_t mutex;
    sem_t empty;
    sem_t full;
} Queue;

Queue q;

//큐를 초기화하고 세마포어를 초기화합니다.
void q_init(Queue *q) {
    q->in = 0;
    q->out = 0;
    sem_init(&q->mutex, 0, 1);   // Mutex for buffer access
    sem_init(&q->empty, 0, BUFFER_SIZE);  // Number of empty slots
    sem_init(&q->full, 0, 0);   // Number of filled slots
}

//생산자가 큐에 데이터를 추가하는 함수로, 빈 슬롯을 기다린 후 데이터를 추가합니다.
void q_enqueue(Queue *q, data_t item) {
    sem_wait(&q->empty);   // Wait for an empty slot
    sem_wait(&q->mutex);   // Acquire mutex to access buffer

    q->buffer[q->in] = item;
    q->in = (q->in + 1) % BUFFER_SIZE;

    sem_post(&q->mutex);   // Release mutex
    sem_post(&q->full);    // Signal that buffer is no longer empty
}

//소비자가 큐에서 데이터를 제거하는 함수로, 데이터가 채워진 슬롯을 기다린 후 데이터를 제거합니다.
data_t q_dequeue(Queue *q) {
    data_t item;

    sem_wait(&q->full);    // Wait for a filled slot
    sem_wait(&q->mutex);   // Acquire mutex to access buffer

    item = q->buffer[q->out];
    q->out = (q->out + 1) % BUFFER_SIZE;

    sem_post(&q->mutex);   // Release mutex
    sem_post(&q->empty);   // Signal that buffer is no longer full

    return item;
}

//생산자 스레드로, 일정 시간마다 랜덤한 데이터를 생성하여 큐에 추가합니다.
void *producer(void *arg) {
    int i;
    for (i = 0; i < 10; i++) {
        data_t item = rand() % 100;   // Generate a random item
        q_enqueue(&q, item);
        printf("Produced: %d\n", item);
        usleep(100000);   // Sleep for 0.1 second
    }
    pthread_exit(NULL);
}

//소비자 스레드로, 일정 시간마다 큐에서 데이터를 제거하여 소비합니다.
void *consumer(void *arg) {
    int i;
    for (i = 0; i < 10; i++) {
        data_t item = q_dequeue(&q);
        printf("Consumed: %d\n", item);
        usleep(200000);   // Sleep for 0.2 second
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t prod_thread, cons_thread;

    q_init(&q);

    // Create producer and consumer threads
    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    // Wait for threads to complete
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    return 0;
}
