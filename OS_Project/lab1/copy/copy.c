// copy.c
// 使用fread和fwrite等库函数实现文件拷贝功能
// 2020.1.17

#include <stdio.h>

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

    FILE *sourceFp = fopen(source, "r");
    if (sourceFp == NULL) {
        printf("open source file failed\n");
        return 0;
    }

    FILE *targetFp = fopen(target, "w");
    if (targetFp == NULL) {
        printf("open target file failed\n");
        fclose(sourceFp);
        return 0;
    }

    char buffer[BUFFER_LENGTH];
    int readLength;

    while ((readLength = fread(buffer, sizeof(char), BUFFER_LENGTH,
                               sourceFp)) == BUFFER_LENGTH) {
        fwrite(buffer, sizeof(char), BUFFER_LENGTH, targetFp);
    }

    if (feof(sourceFp) != 0) {
        fwrite(buffer, sizeof(char), readLength, targetFp);
    } else {
        printf("error while copying file\n");
    }

    fclose(sourceFp);
    fclose(targetFp);

    return 0;
}
