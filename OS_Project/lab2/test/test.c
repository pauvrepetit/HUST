/* 
 * 系统功能调用huao_copy测试程序
 * 添加的系统功能调用号为436
 * 2019-02-15
 */
#include </lib/modules/5.4.19-huao/source/arch/x86/include/generated/uapi/asm/unistd_64.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        // 参数错误
        printf("copy: argument error\n");
        printf("copy <source> <target>\n");
        return 0;
    }

    char *source = argv[1];
    char *target = argv[2];

    if (syscall(436, source, strlen(source), target, strlen(target)) == 0) {
        printf("copy finished\n");
    } else {
        printf("copy failed\n");
    }
    return 0;
}
