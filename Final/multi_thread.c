//0327
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 5

// 스레드에서 실행될 함수
void *thread_function(void *arg) {
    int thread_id = *((int *)arg); // void 포인터로 전달된 인자를 int 형으로 형변환

    printf("Thread %d is running\n", thread_id);

    // 스레드가 종료되기 전까지 대기 (1초)
    sleep(1);

    printf("Thread %d is finished\n", thread_id);

    // 스레드 종료
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int args[NUM_THREADS];

    // 스레드 생성 및 실행
    for (int i = 0; i < NUM_THREADS; i++) {
        args[i] = i;
        if (pthread_create(&threads[i], NULL, thread_function, &args[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    // 모든 스레드가 종료될 때까지 기다림
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            return 1;
        }
    }

    printf("All threads have finished\n");

    return 0;
}


/*
스레드가 생성되고 실행되는 순서는 pthread_create() 함수 호출에 의해 스레드가 생성되는 순서에 의해 결정됩니다. 하지만 각 스레드가 실제로 CPU를 얻어 실행되는 시점은 운영체제 스케줄러가 결정하므로 실행 순서는 예측할 수 없습니다
*/