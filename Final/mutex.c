//0401
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_empty = PTHREAD_COND_INITIALIZER;

/*버퍼에 데이터를 생성하여 넣습니다.
버퍼가 가득 찰 때까지 기다린 후, 뮤텍스를 잠금합니다.
버퍼에 데이터를 추가하고 cond_full을 시그널링하여 버퍼가 데이터로 채워졌음을 알립니다.*/
void *producer(void *arg) {
    int item = 1;
    while (1) {
        pthread_mutex_lock(&mutex);
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&cond_empty, &mutex);
        }
        buffer[count++] = item++;
        printf("Produced item: %d\n", item - 1);
        pthread_cond_signal(&cond_full);
        pthread_mutex_unlock(&mutex);
        sleep(1); // 잠시 멈춤 (생산 속도 조절)
    }
    return NULL;
}

/*버퍼에서 데이터를 소비합니다.
버퍼가 비어 있을 때까지 기다린 후, 뮤텍스를 잠금합니다.
버퍼에서 데이터를 제거하고 cond_empty를 시그널링하여 버퍼가 비어 있음을 알립니다.*/
void *consumer(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (count == 0) {
            pthread_cond_wait(&cond_full, &mutex);
        }
        int item = buffer[--count];
        printf("Consumed item: %d\n", item);
        pthread_cond_signal(&cond_empty);
        pthread_mutex_unlock(&mutex);
        sleep(2); // 잠시 멈춤 (소비 속도 조절)
    }
    return NULL;
}

int main() {
    pthread_t producer_thread, consumer_thread;

    // 생산자 스레드 생성
    if (pthread_create(&producer_thread, NULL, producer, NULL) != 0) {
        perror("pthread_create");
        return 1;
    }

    // 소비자 스레드 생성
    if (pthread_create(&consumer_thread, NULL, consumer, NULL) != 0) {
        perror("pthread_create");
        return 1;
    }

    // 스레드 종료 대기
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    return 0;
}

/*
buffer라는 공유 배열과 count라는 공유 변수를 사용하여 생산자와 소비자 간의 동기화를 구현합니다. mutex는 공유 데이터에 대한 잠금을 관리하고, cond_full과 cond_empty는 버퍼가 가득 차거나 비어있는 상태를 나타내는 조건 변수입니다.

 생산자는 버퍼에 데이터를 넣고 cond_full을 시그널링하여 소비자에게 데이터를 사용할 수 있음을 알립니다. 반대로 소비자는 버퍼에서 데이터를 읽고 cond_empty를 시그널링하여 생산자에게 버퍼가 비었음을 알립니다.
  */