/*
 * 设备驱动测试程序
 * 打开文件后依次写入、读取数据
 * 设备位置为/dev/huao_device，需要在root/sudo环境下执行
 * 2019-02-16
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
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
        return -1;
    }
    for (int i = 1; i <= 100; i++) {
        sprintf(buffer, "This is huao device, number is %d", i);
        length = write(huao_dev, buffer, strlen(buffer));
        if (length == -1) {
            printf("write errno:%s\n", strerror(errno));
        }
        printf("write to device:\t%s\n", buffer);
        memset(buffer, 0, sizeof(char) * 100);
        printf("clear buffer, now buffer is:\t%s\n", buffer);
        sleep(1);
        length = read(huao_dev, buffer, sizeof(buffer));
        if (length == -1) {
            printf("read errno:%s\n", strerror(errno));
        }
        printf("read from device:\t%s\n", buffer);
        sleep(1);
        memset(buffer, 0, sizeof(char) * 100);
    }
    close(huao_dev);
    return 0;
}