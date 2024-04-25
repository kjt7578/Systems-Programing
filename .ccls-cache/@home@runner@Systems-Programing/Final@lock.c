#include <stdio.h>
#include <pthread.h>

pthread_mutex_t lock;
int shared_data = 0;
/*
pthread_mutex_t 타입의 lock 변수는 뮤텍스를 나타냅니다. shared_data는 여러 스레드가 공유하는 데이터로, 이 예시에서는 간단히 정수로 표현됩니다.*/


void *thread_function(void *arg) {
    // 임계 영역 진입 전 뮤텍스 획득
    pthread_mutex_lock(&lock);

    // 공유 데이터 접근 및 수정
    shared_data++;
    printf("Thread updated shared data: %d\n", shared_data);

    // 임계 영역을 벗어나고 뮤텍스 해제
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}
/*
thread_function은 각 스레드가 실행할 함수입니다. 먼저 pthread_mutex_lock(&lock);을 사용하여 뮤텍스를 획득합니다. 이로써 임계 영역에 진입하여 공유 데이터인 shared_data를 증가시키고, 변경된 값을 출력합니다. 마지막으로 pthread_mutex_unlock(&lock);을 호출하여 뮤텍스를 해제하고 임계 영역을 빠져나옵니다.*/

int main() {
    pthread_t threads[2];

    // 뮤텍스 초기화
    pthread_mutex_init(&lock, NULL);

    // 두 개의 스레드 생성
    for (int i = 0; i < 2; i++) {
        pthread_create(&threads[i], NULL, thread_function, NULL);
    }

    // 모든 스레드의 실행이 종료될 때까지 대기
    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }

    // 뮤텍스 제거
    pthread_mutex_destroy(&lock);

    return 0;
}
/*두 개의 스레드를 생성하고 각 스레드를 thread_function으로 실행합니다. 뮤텍스는 pthread_mutex_init(&lock, NULL);을 통해 초기화하고, 스레드가 모두 종료될 때까지 pthread_join을 사용하여 기다립니다. 마지막으로 pthread_mutex_destroy(&lock);를 호출하여 뮤텍스를 제거합니다.*/

/*
위의 예시 코드에서는 두 개의 스레드가 공유 데이터에 안전하게 접근하도록 뮤텍스를 사용하고 있습니다. 각 스레드는 임계 영역에 진입하기 전에 뮤텍스를 획득하고, 임계 영역을 벗어나면 뮤텍스를 해제합니다.

흐름
pthread_mutex_lock(&lock);은 뮤텍스를 획득하기 위해 호출됩니다. 다른 스레드가 뮤텍스를 이미 획득한 경우 해당 스레드는 블록됩니다.
임계 영역에서는 shared_data를 증가시키고, 변경된 값을 출력합니다.
pthread_mutex_unlock(&lock);은 뮤텍스를 해제하여 다른 스레드가 임계 영역에 접근할 수 있
*/