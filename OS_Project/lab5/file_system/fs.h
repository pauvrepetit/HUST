#ifndef FS_H
#define FS_H
// 我们用16个block来存储位示图可以表示 16 × 4 × 1024 × 8 = 512K 个block，即2G的空间
// 我们把superBlock存在第0块，接下来存储位示图(1-16块)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ui_mainwindow.h"
#include "mainwindow.h"

#define POINTER_LENGTH sizeof(void *)
#define BLOCK_SIZE 4096

// 文件的索引结构，256Bytes
// 一个block是4KB，可以存储16个文件索引
struct FileNode {
    char filename[160];  // 160 Bytes
    unsigned long dataPoint[10]; // 8 * 10 Bytes    // 存储指向块的块号
    unsigned long blockPoint;    // 8 Bytes
    unsigned int size;   // 4 Bytes
    short type;          // 2 Bytes
    char valid;          // 1 Bytes
};

struct SuperBlock {
    struct FileNode root;   // 我们在superBlock中保存根目录的索引结构，在格式化的时候创建
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

unsigned long getBlock(FILE *);
int releaseBlock(unsigned long, FILE *);

extern QString originText;

#endif // FS_H
