#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

pthread_cond_t g_condition;  // 조건변수는 단지 상태정보만을 알리기 위해서 사용되며, (※ 조건변수는 Lock 기능 없음.)
pthread_mutex_t g_mutex;     // 상태정보를 원자적(atomic)으로 주고받기 위해서는 뮤텍스와 함께 사용해야 한다.

void *ThWaitSig()
{
    struct timeval! now;
    struct timespec ts;

    printf("start ThWaitSig..\n");
    gettimeofday(&now, NULL);
    ts.tv_sec = now.tv_sec + 5;
    ts.tv_nsec = now.tv_usec * 1000;

    pthread_mutex_lock(&g_mutex);
    printf("now waiting wakeup signal about 5 sec..\n");
    pthread_cond_timedwait(&g_condition, &g_mutex, &ts);
    printf("ok, i'm wakeup..\n");
    pthread_mutex_unlock(&g_mutex);
    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t thread;

    pthread_mutex_init(&g_mutex, NULL);
    pthread_cond_init(&g_condition, NULL);

    pthread_create(&thread, NULL, ThWaitSig, NULL);

    pthread_mutex_lock(&g_mutex);
    sleep(1);
    printf("now send wakeup signal.. \n");
    pthread_cond_signal(&g_condition);
    pthread_mutex_unlock(&g_mutex);

    pthread_join(thread, NULL);
    printf("thread's working is done.\n");

    pthread_cond_destroy(&g_condition);
    pthread_mutex_destroy(&g_mutex);
    return 0;
}


출처: http://unabated.tistory.com/entry/pthreadcondtimedwait를-사용한-이벤트-동기화-사용-예제 [랄라라]