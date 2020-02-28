#ifndef FS_H
#define FS_H
// 我们用16个block来存储位示图可以表示 16 × 4 × 1024 × 8 = 512K
// 个block，即2G的空间 我们把superBlock存在第0块，接下来存储位示图(1-16块)
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

#define POINTER_LENGTH sizeof(void *)
#define BLOCK_SIZE 4096
#define MAX_USER_NUM 10
#define MAX_USER_LEN 16

// 文件的索引结构，256Bytes
// 一个block是4KB，可以存储16个文件索引
struct FileNode {
    char filename[160];          // 160 Bytes
    unsigned long dataPoint[10]; // 8 * 10 Bytes    // 存储指向块的块号
    unsigned long blockPoint;    // 8 Bytes
    unsigned int size;           // 4 Bytes
    short type;                  // 2 Bytes
    char valid;                  // 1 Bytes
    char belong;                 // 1 Bytes         // 文件所属的用户的uid
};

struct SuperBlock {
    struct FileNode root;
    // 我们在superBlock中保存根目录的索引结构，在格式化的时候创建
    char username[MAX_USER_NUM][16];
    unsigned char password[MAX_USER_NUM][MD5_DIGEST_LENGTH];
    int blockNumber;
    int blockSize;
};

int format(FILE *);
int createFile(const char *, FILE *);
int createDir(const char *, FILE *);
long openFile(const char *, FILE *, Ui::MainWindow *);
int writeClose(const char *, long, FILE *);
int listFile(const char *, FILE *, Ui::MainWindow *);
int rmFile(const char *, FILE *);
int rmDir(const char *, FILE *);

int signin(const char *, const char *, FILE *);
int login(const char *, const char *, FILE *);
int logout();

int shareFile(const char *, FILE *);

unsigned long getBlock(FILE *);
int releaseBlock(unsigned long, FILE *);

extern QString originText;
extern char currentUid;

#endif // FS_H
