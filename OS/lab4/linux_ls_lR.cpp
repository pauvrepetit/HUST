// 实现linux中 ls -laR 的功能

#include "color.h"
#include <algorithm>
#include <cstdio>
#include <dirent.h>
#include <grp.h>
#include <iostream>
#include <pwd.h>
#include <queue>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

using std::queue;
using std::sort;
using std::string;
using std::vector;

char username[256];
char groupname[256];

bool entry_compare(dirent *a, dirent *b) {
    if(strcmp(a->d_name, b->d_name) >= 0) {
        return 0;
    } else {
        return 1;
    }
}

int printMode(__mode_t mod) {
    if (mod & __S_IREAD) {
        putchar('r');
    } else {
        putchar('-');
    }
    if (mod & __S_IWRITE) {
        putchar('w');
    } else {
        putchar('-');
    }
    if (mod & __S_IEXEC) {
        putchar('x');
    } else {
        putchar('-');
    }

    if (mod & (__S_IREAD >> 3)) {
        putchar('r');
    } else {
        putchar('-');
    }
    if (mod & (__S_IWRITE >> 3)) {
        putchar('w');
    } else {
        putchar('-');
    }
    if (mod & (__S_IEXEC >> 3)) {
        putchar('x');
    } else {
        putchar('-');
    }
    if (mod & (__S_IREAD >> 6)) {
        putchar('r');
    } else {
        putchar('-');
    }
    if (mod & (__S_IWRITE >> 6)) {
        putchar('w');
    } else {
        putchar('-');
    }
    if (mod & (__S_IEXEC >> 6)) {
        putchar('x');
    } else {
        putchar('-');
    }
    if (mod & __S_IEXEC) {
        return 1;
    } else {
        return 0;
    }
}

void getUser(uid_t uid, char *username) {
    struct passwd *p = getpwuid(uid);
    strcpy(username, p->pw_name);
    return;
}

void getGroup(gid_t gid, char *username) {
    struct group *p = getgrgid(gid);
    strcpy(username, p->gr_name);
    return;
}

void printTime(struct timespec lastModTime) {
    time_t time_s = lastModTime.tv_sec;
    struct tm *time_tm = gmtime(&time_s);
    char *time_str = ctime(&time_s);
    int i = 0;
    while (time_str[i] != '\n') {
        i++;
    }
    time_str[i] = '\0';
    printf("%s ", time_str);
    return;
}

int printInfo(struct stat statbuf, int depth) {
    int r = printMode(statbuf.st_mode);
    printf(" %d ", depth);
    getUser(statbuf.st_uid, username);
    getGroup(statbuf.st_gid, groupname);
    printf("%s %s\t", username, groupname);
    printf("%ld\t", statbuf.st_size);
    printTime(statbuf.st_mtim);
    return r;
}

void printDIR(const char *dir, int depth) {
    DIR *dp;              // 目录指针
    struct dirent *entry; // 目录项结构
    struct stat statbuf;  // 文件信息
    char sLinkFilename[1024];
    int sLinkLen;

    string strip("/");

    queue<string> subDir;
    vector<dirent *> entry_list;

    dp = opendir(dir); // 打开目录
    if (dp == NULL) {
        printf("open dir %s failed\n", dir);
        return;
    }

    printf("%s\n", dir);

    while ((entry = readdir(dp)) != NULL) {
        entry_list.push_back(entry);
    }

    sort(entry_list.begin(), entry_list.end(), entry_compare);

    for (int i = 0; i < entry_list.size(); i++) {
        entry = entry_list[i];
        string subdir(entry->d_name);
        lstat((dir + strip + subdir).c_str(), &statbuf);

        switch (statbuf.st_mode & __S_IFMT) {
            // 这里的st_mode里面包含了很多信息
            // 其中用__S_IFMT作为掩码获取与文件类型相关的部分
        case S_IFDIR:
            // 目录
            putchar('d');
            printInfo(statbuf, depth);
            printf(L_BLUE "%s" NONE "\n", subdir.c_str());
            if (strcmp(subdir.c_str(), ".") == 0 ||
                strcmp(subdir.c_str(), "..") == 0) {
                // ignore
                break;
            }
            subDir.push(dir + strip + subdir);
            break;
        case S_IFLNK:
            // 软链接
            putchar('l');
            printInfo(statbuf, depth);
            sLinkLen = readlink((dir + strip + subdir).c_str(), sLinkFilename, 1024);
            sLinkFilename[sLinkLen] = 0;
            printf(L_GREEN "%s -> " GREEN "%s" NONE "\n", subdir.c_str(),
                   sLinkFilename);
            break;
        case S_IFREG:
            // 普通文件
            putchar('-');
            if (printInfo(statbuf, depth)) {
                printf(L_GREEN "%s" NONE "\n", subdir.c_str());
            } else {
                printf("%s\n", subdir.c_str());
            }
            break;
        default:
            break;
        }
    }

    putchar('\n');

    string d;
    while (!subDir.empty()) {
        d = subDir.front();
        subDir.pop();
        printDIR(d.c_str(), depth + 1);
    }

    closedir(dp);

    return;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printDIR(".", 1);
    } else {
        printDIR(argv[1], 1);
    }
    return 0;
}