// 有一个问题
// 用ulimit将程序内存空间限制到256K(262144)
// 每个线程栈空间为8K(8192)
// 此时创建31个子线程时(包含主线程共32个线程)
// 程序可以正常运行
// 此时如果创建32个或更多的子线程，在调用pthread_create函数创建线程时，不会出问题
// 但是在运行的过程中会出现段错误
//
// pthread_create函数调用时不会分配所有资源？
// 能否解决？
//
// 虽然pthread_create返回0 但是 设置的threadID是0??
// 为什么会这样呢


#include <pthread.h>
#include <stdio.h>
#include <sys/sem.h>
#include <unistd.h>

#define THREAD_NUM 3
#define TICKET_NUM 20

int sale;
int semid;
pthread_t thread_id[THREAD_NUM];

union semun {
    int val;               // SETVAL使用的值
    struct semid_ds *buf;  // IPC_STAT、IPC_SET 使用缓存区
    unsigned short *array; // GETALL、SETALL 使用的数组
    struct seminfo *__buf; // IPC_INFO(Linux特有) 使用缓存区
};

// P 操作
void P(int semid, int index) {
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = -1;
    sem.sem_flg = 0;
    semop(semid, &sem, 1);
    return;
}

// V 操作
void V(int semid, int index) {
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = 1;
    sem.sem_flg = 0;
    semop(semid, &sem, 1);
    return;
}

void *saleTicket(void *arg) {
    // printf("thread %ld func started\n", (pthread_t *)arg - thread_id);
    // pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    // 收到cancel信号时运行到终止点再终止

    usleep(100); // 睡100ms

    while (1) {
        pthread_testcancel(); // 设置终止点
        P(semid, 0);
        if (sale >= TICKET_NUM) {
            printf("thread \t %ld\t\tno ticket, thread exit\n",
                   (pthread_t *)arg - thread_id);
            V(semid, 0);
            return NULL;
        }
        sale++;
        printf("sale thread \t %ld\t\tsales ticket \t%d\n",
               (pthread_t *)arg - thread_id, sale);
        V(semid, 0);
    }
}

int main() {
    sale = 0; // 开始时，已售票数量为0

    // 创建信号灯
    semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        printf("semget failed\n");
        return 0;
    }

    semun arg;
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        semctl(semid, 0, IPC_RMID);
        printf("init sem 0 failed\n");
        return 0;
    }
    // 给信号灯赋初值

    for (int i = 0; i < THREAD_NUM; i++) {
        // printf("it's time to create thread %d\n", i);
        if (pthread_create(thread_id + i, NULL, saleTicket,
                           (void *)&(thread_id[i])) == -1 ||
            thread_id[i] == 0) {
            printf("thread %d create failed\n", i);
            for (int j = 0; j < i; j++) {
                pthread_cancel(thread_id[j]);
            }
            for (int j = 0; j < i; j++) {
                pthread_join(thread_id[j], NULL);
                printf("thread %d is canceled\n", j);
            }
            // 终止所有线程 并等待线程终止完成
            return 0;
        }
        printf("thread %d create successful threadID is %ld\n", i,
               thread_id[i]);
    }

    // 所有售票线程创建完成

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(thread_id[i], NULL);
        printf("wait thread %d successful\n", i);
    }

    semctl(semid, 0, IPC_RMID); // 删除信号灯集

    printf("finished\n");

    return 0;
}