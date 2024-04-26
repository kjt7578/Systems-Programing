//0408
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*
  readers: 현재 읽기 작업을 수행 중인 스레드 수를 나타내는 카운터
  writers: 현재 쓰기 작업을 수행 중인 스레드 수를 나타내는 카운터
  lock: 뮤텍스(mutex)를 사용하여 잠금을 제어하는 데 사용되는 객체
  read_ready: 읽기 작업을 위해 대기하는 조건 변수
  write_ready: 쓰기 작업을 위해 대기하는 조건 변수 */
typedef struct {
    int readers;
    int writers;
    pthread_mutex_t lock;
    pthread_cond_t read_ready;
    pthread_cond_t write_ready;
} rw_lock_t;

//rw_lock_t 객체를 초기화합니다. 읽기/쓰기 카운터를 0으로 설정하고, 뮤텍스와 조건 변수들을 초기화합니다.
void rw_lock_init(rw_lock_t *rw) {
    rw->readers = 0;
    rw->writers = 0;
    pthread_mutex_init(&rw->lock, NULL);
    pthread_cond_init(&rw->read_ready, NULL);
    pthread_cond_init(&rw->write_ready, NULL);
}

//읽기 작업을 수행하기 위해 호출됩니다. 현재 쓰기 작업이 진행 중인 경우(즉, writers > 0), 읽기 작업을 위해 대기하게 됩니다. 읽기 카운터를 증가시키고, 뮤텍스를 다시 잠금 해제합니다.
void rw_lock_read_lock(rw_lock_t *rw) {
    pthread_mutex_lock(&rw->lock);
    while (rw->writers > 0) {
        pthread_cond_wait(&rw->read_ready, &rw->lock);
    }
    rw->readers++;
    pthread_mutex_unlock(&rw->lock);
}

//읽기 작업을 마치고 잠금을 해제하는데 사용됩니다. 읽기 카운터를 감소시키고, 모든 읽기 작업이 완료된 경우(즉, readers == 0), 쓰기 작업을 위해 대기 중인 스레드에게 시그널을 보냅니다
void rw_lock_read_unlock(rw_lock_t *rw) {
    pthread_mutex_lock(&rw->lock);
    rw->readers--;
    if (rw->readers == 0) {
        pthread_cond_signal(&rw->write_ready);
    }
    pthread_mutex_unlock(&rw->lock);
}

//쓰기 작업을 수행하기 위해 호출됩니다. 현재 읽기 또는 쓰기 작업이 진행 중인 경우(즉, readers > 0 또는 writers > 0), 쓰기 작업을 위해 대기하게 됩니다. 쓰기 카운터를 증가시키고, 뮤텍스를 다시 잠금 해제합니다.
void rw_lock_write_lock(rw_lock_t *rw) {
    pthread_mutex_lock(&rw->lock);
    while (rw->readers > 0 || rw->writers > 0) {
        pthread_cond_wait(&rw->write_ready, &rw->lock);
    }
    rw->writers++;
    pthread_mutex_unlock(&rw->lock);
}

//쓰기 작업을 마치고 잠금을 해제하는데 사용됩니다. 쓰기 카운터를 감소시키고, 읽기 작업을 위해 대기 중인 스레드에게 시그널을 보내고, 쓰기 작업을 위해 대기 중인 모든 스레드에게 시그널을 보냅니다.
void rw_lock_write_unlock(rw_lock_t *rw) {
    pthread_mutex_lock(&rw->lock);
    rw->writers--;
    pthread_cond_signal(&rw->read_ready);
    pthread_cond_broadcast(&rw->write_ready);
    pthread_mutex_unlock(&rw->lock);
}

void *reader_thread(void *arg) {
    rw_lock_t *rw = (rw_lock_t *)arg;
    rw_lock_read_lock(rw);
    printf("Reader is reading...\n");
    rw_lock_read_unlock(rw);
    return NULL;
}

void *writer_thread(void *arg) {
    rw_lock_t *rw = (rw_lock_t *)arg;
    rw_lock_write_lock(rw);
    printf("Writer is writing...\n");
    rw_lock_write_unlock(rw);
    return NULL;
}

//메인 함수에서는 rw_lock_t 객체를 초기화하고, reader 스레드와 writer 스레드를 생성하여 실행합니다. reader 스레드는 rw_lock_read_lock과 rw_lock_read_unlock을 호출하고, writer 스레드는 rw_lock_write_lock과 rw_lock_write_unlock을 호출하여 읽기/쓰기 작업을 수행합니다.

int main() {
    rw_lock_t rw;
    rw_lock_init(&rw);

    pthread_t readers[3];
    pthread_t writers[2];

    // Create reader threads
    for (int i = 0; i < 3; i++) {
        pthread_create(&readers[i], NULL, reader_thread, (void *)&rw);
    }

    // Create writer threads
    for (int i = 0; i < 2; i++) {
        pthread_create(&writers[i], NULL, writer_thread, (void *)&rw);
    }

    // Join reader threads
    for (int i = 0; i < 3; i++) {
        pthread_join(readers[i], NULL);
    }

    // Join writer threads
    for (int i = 0; i < 2; i++) {
        pthread_join(writers[i], NULL);
    }

    return 0;
}
