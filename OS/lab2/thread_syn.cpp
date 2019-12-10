#include <pthread.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

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

int count;
int semid; // 信号灯ID

void *calculate_func(void *arg) {
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    // 收到cancel信号时运行到终止点再终止
    while (1) {
        pthread_testcancel(); // 设置终止点
        P(semid, 1);
        if (count > 100) {
            V(semid, 0);
            return NULL;
        }
        printf("calculate thread add count from %d to %d\n", count, count + 1);
        count++;
        V(semid, 0);
    }

    return NULL;
}

void *print_func(void *arg) {
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    // 收到cancel信号时运行到终止点再终止
    while (1) {
        pthread_testcancel(); // 设置终止点
        P(semid, 0);
        if (count > 100) {
            V(semid, 1);
            return NULL;
        }
        printf("print by thread 2\t\t%d\n", count);
        V(semid, 1);
    }

    return NULL;
}

int main() {
    // 创建信号灯
    semid = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);
    if (semid == -1) {
        printf("semget failed\n");
        return 0;
    }

    semun arg;
    arg.val = 0;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        printf("init sem 0 failed\n");
        semctl(semid, 0, IPC_RMID);
        return 0;
    }
    arg.val = 1;
    if (semctl(semid, 1, SETVAL, arg) == -1) {
        printf("init sem 1 failed\n");
        semctl(semid, 0, IPC_RMID);
        return 0;
    }
    // 给信号灯赋初值

    count = 0;

    pthread_t calculate, print;
    if (pthread_create(&calculate, NULL, calculate_func, NULL) == -1) {
        printf("create calculate thread failed\n");
        return 0;
    }

    if (pthread_create(&print, NULL, print_func, NULL) == -1) {
        printf("create print thread failed\n");
        pthread_cancel(calculate);
        pthread_join(calculate, NULL);
        // 终止计算线程并等待其结束
        return 0;
    }

    // 计算和输出线程创建成功

    pthread_join(calculate, NULL);
    pthread_join(print, NULL);

    semctl(semid, 0, IPC_RMID);

    printf("\nfinished\n");

    return 0;
}