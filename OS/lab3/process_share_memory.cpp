#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/timeb.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define SHM_LEN 10000
#define SHM_NUM 5
#define INIT_SEM SHM_NUM
#define INPUT_FILE ("input.txt")
#define OUTPUT_FILE ("output.txt")
// #define INPUT_FILE ("/mnt/ramfs/output.txt")

struct buffer {
    int len;
    char b[SHM_LEN];
};

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

int getBufLen(char *);

int main(void) {
    // 创建共享内存
    int shmid[SHM_NUM];
    shmid_ds buf;

    for (int i = 0; i < SHM_NUM; i++) {
        shmid[i] = shmget(IPC_PRIVATE, sizeof(struct buffer), IPC_CREAT | 0666);
        if (shmid[i] == -1) {
            printf("create share memory %d failed\n", i);
            for (int j = 0; j < i; j++) {
                shmctl(shmid[j], IPC_RMID, &buf);
            }

            return 0;
        }
    }

    // 创建信号灯
    int read_semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (read_semid == -1) { // 信号灯创建失败
        printf("create read sem failed\n");
        return 0;
    }

    int write_semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (write_semid == -1) {
        printf("create write sem failed\n");
        return 0;
    }

    // 信号灯赋初值
    semun read_arg, write_arg;
    read_arg.val = 0;
    write_arg.val = INIT_SEM;
    if (semctl(read_semid, 0, SETVAL, read_arg) == -1 ||
        semctl(write_semid, 0, SETVAL, write_arg) == -1) {
        // 信号灯初始化失败
        semctl(read_semid, 0, IPC_RMID);
        semctl(write_semid, 0, IPC_RMID);
        printf("init sem failed\n");
        return 0;
    }

    __pid_t readbuf, writebuf;
    clock_t begin_clock, end_clock;
    begin_clock = clock();
    timeb begin, end;
    ftime(&begin);
    readbuf = fork();
    if (readbuf == -1) {
        printf("main::create process readbuf failed\n");
        return 0;
    }
    if (readbuf != 0) {
        // main
        // printf("main::create readbuf process success\n");
        writebuf = fork();
        if (writebuf == -1) {
            printf("main::create process writebuf failed\n");
            kill(readbuf, SIGINT);
            return 0;
        }
        if (writebuf != 0) {
            // main
            // printf("main::create writebuf process success\n");
            waitpid(readbuf, NULL, 0);
            printf("main::process readbuf exit\n");
            waitpid(writebuf, NULL, 0);
            printf("main::process writebuf exit\n");
            // 等两个进程结束
            // 删除共享内存空间
            for (int i = 0; i < SHM_NUM; i++) {
                shmctl(shmid[i], IPC_RMID, &buf);
            }
            // 删除信号灯
            semctl(read_semid, 0, IPC_RMID);
            semctl(write_semid, 0, IPC_RMID);
            printf("main::process main exit\n");

            ftime(&end);
            end_clock = clock();
            printf("main::CPU time is %lf ms, run time is %lld ms\n",
                   1000.0 * (end_clock - begin_clock) / CLOCKS_PER_SEC,
                   (end.time - begin.time) * 1000 + end.millitm -
                       begin.millitm);
            return 0;
        } else {
            // write process
            printf("writebuf::process started\n");
            begin_clock = clock();

            // char *writeBuffer[SHM_NUM];
            struct buffer *writeBuffer[SHM_NUM];
            // detach共享内存
            for (int i = 0; i < SHM_NUM; i++) {
                writeBuffer[i] =
                    (struct buffer *)shmat(shmid[i], NULL, IPC_CREAT | 0666);
            }

            FILE *fp = fopen(INPUT_FILE, "r");
            if (fp == NULL) {
                printf("In writebuf process, open file error\n");
                kill(readbuf, SIGINT);
                return 0;
            }

            int count = 0;
            int writeSize = 0;
            while (1) {
                P(write_semid, 0);
                writeBuffer[count]->len =
                    fread(writeBuffer[count]->b, sizeof(char), SHM_LEN, fp);
                if (writeBuffer[count]->len == 0) {
                    printf("writebuf::write file data finished, "
                           "process exit\n");
                    V(read_semid, 0);
                    fclose(fp);
                    ftime(&end);
                    end_clock = clock();
                    printf(
                        "writebuf::CPU time is %lf ms, run time is %lld ms\n",
                        1000.0 * (end_clock - begin_clock) / CLOCKS_PER_SEC,
                        (end.time - begin.time) * 1000 + end.millitm -
                            begin.millitm);
                    exit(0);
                }

                // printf("writebuf::%s\n", writeBuffer[count]);
                V(read_semid, 0);
                count = (count + 1) % SHM_NUM;
            }
        }
    } else {
        // read process
        printf("readbuf::process started\n");
        begin_clock = clock();
        struct buffer *readBuffer[SHM_NUM];
        // char *readBuffer[SHM_NUM];
        // attach共享内存
        for (int i = 0; i < SHM_NUM; i++) {
            readBuffer[i] =
                (struct buffer *)shmat(shmid[i], NULL, IPC_CREAT | 0666);
        }

        FILE *fp = fopen(OUTPUT_FILE, "w");
        if (fp == NULL) {
            printf("In readbuf process, open file error\n");
            kill(writebuf, SIGINT);
            return 0;
        }

        int count = 0;
        while (1) {
            P(read_semid, 0);
            if (readBuffer[count]->len == 0) {
                printf("readbuf::read file data finished, process exit\n");
                V(write_semid, 0);
                fclose(fp);
                ftime(&end);
                end_clock = clock();
                printf("readbuf::CPU time is %lf ms, run time is %lld ms\n",
                       1000.0 * (end_clock - begin_clock) / CLOCKS_PER_SEC,
                       (end.time - begin.time) * 1000 + end.millitm -
                           begin.millitm);
                exit(0);
            }

            fwrite(readBuffer[count]->b, sizeof(char), readBuffer[count]->len, fp);
            // printf("readbuf::%s\n", readBuffer[count]);
            V(write_semid, 0);
            count = (count + 1) % SHM_NUM;
        }
    }

    return 0;
}
