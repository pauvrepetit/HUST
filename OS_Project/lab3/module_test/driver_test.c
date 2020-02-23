/*
 * 设备驱动测试程序
 * 打开文件后依次写入、读取数据
 * 设备位置为/dev/huao_device，需要在root/sudo环境下执行
 * 2019-02-16
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    char buffer[100];
    int length;
    int huao_dev = open("/dev/huao_device", O_RDWR); // 打开文件 可读可写
    if (huao_dev == -1) {
        printf("open device errno:%s\n", strerror(errno));
        printf("if the device does not exist, use dmesg to get major number of "
               "huao_device\n");
        printf(
            "use 'mknod /dev/huao_device c <major> 0' to create the device\n");
        printf("if permission denied, use sudo to run the test program\n");
        return -1;
    }
    int type;
    char *writeStr;
    size_t writeLen = 0;
    while (1) {
        printf("Read(0) Or Write(1) or Exit(-1)? ");
        scanf("%d", &type);
        getchar();
        if (type == 0) {
            memset(buffer, 0, 100);
            length = read(huao_dev, buffer, 100);
            if (length == -1) {
                printf("write errno:%s\n", strerror(errno));
                break;
            }
            printf("Read from huao_device: %s\n", buffer);
        } else if (type == 1) {
            printf("Input a string to write: ");
            writeStr = NULL;
            writeLen = 0;
            getline(&writeStr, &writeLen, stdin);
            writeStr[strlen(writeStr) - 1] = 0;     // getline会把换行符读进去 把\n替换掉
            strcpy(buffer, writeStr);
            free(writeStr);
            length = write(huao_dev, buffer, strlen(buffer));
            if (length == -1) {
                printf("write errno:%s\n", strerror(errno));
            }
        } else if (type == -1) {
            break;
        } else {
            printf("input error\n");
            continue;
        }
    }
    close(huao_dev);
    return 0;
}
