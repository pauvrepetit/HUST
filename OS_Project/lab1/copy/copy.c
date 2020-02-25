/*
 * lab1 copy file
 * 使用系统功能调用完成文件拷贝功能
 * open/close/read/write
 * 2020-02-25
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_LENGTH 1024

int main(int argc, char **argv) {
    if (argc != 3) {
        // 参数错误
        printf("copy: argument error\n");
        printf("copy <source> <target>\n");
        return 0;
    }

    char *source = argv[1];
    char *target = argv[2];

    // 打开文件
    int srcFile = open(source, O_RDONLY);
    if (srcFile == -1) {
        printf("open source file failed, error info: %s\n", strerror(errno));
        return -1;
    }

    int dstFile = open(target, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (dstFile == -1) {
        printf("open target file failed, error info: %s\n", strerror(errno));
        close(srcFile);
        return -1;
    }

    // 复制文件
    char buffer[BUFFER_LENGTH];
    char *bufferPtr;
    int readLength, writeLength;
    int error = 0;

    while((readLength = read(srcFile, buffer, BUFFER_LENGTH)) && (error == 0)) {
        if(readLength == -1) {
            // 读文件发生致命错误
            printf("read file failed, error info: %s\n", strerror(errno));
            error = 1;
            break;
        } else if(readLength == 0) {
            // 文件读完了
            break;
        } else {
            // 从文件中读取到readLength个字节的数据 下面我们把这些数据写入到dstFile中
            bufferPtr = buffer;
            while((writeLength = write(dstFile, bufferPtr, readLength))) {
                if(writeLength == -1) {
                    // 写文件发生致命错误
                    printf("write file failed, error info: %s\n", strerror(errno));
                    error = 2;
                    break;
                } else if(writeLength == readLength) {
                    // 写完了buffer中所有的数据
                    break;
                } else if(writeLength < readLength) {
                    // 写文件出错，没有将readLength个字节的数据完全写入
                    // 该错误可以处理
                    bufferPtr += writeLength;
                    readLength -= writeLength;
                }
            }
        }
    }

    close(srcFile);
    close(dstFile);
    if(error == 1) {
        printf("copy file failed, a fatal error occerred when reading file\n");
        return 1;
    } else if(error == 2) {
        printf("copy file failed, a fatal error occerred when writing file\n");
        return 2;
    } else {
        printf("copy file successful\n");
        return 0;
    }
}