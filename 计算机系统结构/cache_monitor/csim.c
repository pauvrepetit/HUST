#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cacheList {
    unsigned long long tag; // 保存内存地址中的tag字段
    struct cacheList *next;
};

struct cacheLine {
    int count;               // 保存该cache行中已经存在的块数量
    struct cacheList *first; // 保存第一个地址
};

void printHelp();
int setPara(char **argv, int i, int *s, int *E, int *b, char **file);
int cacheLoad(struct cacheLine *cache, char *line, int s, int E, int b,
              int verbose, int *countHit, int *countMiss, int *countEviction);
int cacheModify(struct cacheLine *cache, char *line, int s, int E, int b,
                int verbose, int *countHit, int *countMiss, int *countEviction);

int main(int argc, char **argv) {
    int verbose = 0;
    int s, E, b;
    char *file;
    // 首先来判断参数
    if (argc < 9 || argc > 10) {
        printHelp();
        return 0;
    }
    if (strcmp(argv[1], "-h") == 0) {
        printHelp();
        return 0;
    }
    verbose = argc - 9;
    if (setPara(argv, argc - 8, &s, &E, &b, &file) != 0) {
        printHelp();
        return 0;
    }

    struct cacheLine *cache =
        (struct cacheLine *)malloc(sizeof(struct cacheLine) * (1 << s));
    memset(cache, 0, sizeof(struct cacheLine) * (1 << s));

    FILE *fp = fopen(file, "r");
    char mod[3];
    char line[256];
    int countHit = 0;
    int countMiss = 0;
    int countEviction = 0;
    fgets(line, 256, fp);
    while (!feof(fp)) {
        line[strlen(line) - 1] = 0;
        sscanf(line, "%s", mod);
        if (strcmp(mod, "I") == 0) {
            fgets(line, 256, fp);
            continue;
        }
        if (strcmp(mod, "L") == 0)
            cacheLoad(cache, line, s, E, b, verbose, &countHit, &countMiss,
                      &countEviction);
        else if (strcmp(mod, "S") == 0)
            cacheLoad(cache, line, s, E, b, verbose, &countHit, &countMiss,
                      &countEviction);
        else if (strcmp(mod, "M") == 0)
            cacheModify(cache, line, s, E, b, verbose, &countHit, &countMiss,
                        &countEviction);
        fgets(line, 256, fp);
    }

    printSummary(countHit, countMiss, countEviction);
    return 0;
}

int cacheLoad(struct cacheLine *cache, char *line, int s, int E, int b,
              int verbose, int *countHit, int *countMiss, int *countEviction) {
    unsigned long long addr;
    char mod[3];
    sscanf(line, "%s %llx", mod, &addr);
    unsigned long long tag = addr & ~((1 << (s + b)) - 1);
    unsigned long long index = (addr & ((1 << (s + b)) - 1)) >> b;
    struct cacheList *first = cache[index].first;
    struct cacheList *second = NULL;
    while (first != NULL) {
        if (first->tag == tag) {
            // 命中
            if (second != NULL) {
                // 将first的块调整为第一个块
                second->next = first->next;
                first->next = cache[index].first;
                cache[index].first = first;
            }
            (*countHit)++;
            if (verbose == 1)
                printf("%s hit\n", line);
            return 1;
        } else {
            second = first;
            first = first->next;
        }
    }
    // 如果到这还没有命中的话 那么我们就需要将该页添加到cache中了
    (*countMiss)++;
    if (cache[index].count == E) {
        // 说明cache满了 需要替换
        first = cache[index].first;
        if (E == 1) {
            first->tag = tag;
        } else {
            while (first->next != NULL) {
                second = first;
                first = first->next;
            }
            second->next = NULL;
            first->tag = tag;
            first->next = cache[index].first;
            cache[index].first = first;
        }
        (*countEviction)++;
        if (verbose == 1)
            printf("%s miss eviction\n", line);
        return 3;
    } else {
        // cache没满 直接加入
        first = (struct cacheList *)malloc(sizeof(struct cacheList));
        first->next = cache[index].first;
        first->tag = tag;
        cache[index].first = first;
        cache[index].count++;
        if (verbose == 1)
            printf("%s miss\n", line);
        return 2;
    }
}

int cacheModify(struct cacheLine *cache, char *line, int s, int E, int b,
                int verbose, int *countHit, int *countMiss,
                int *countEviction) {
    int load =
        cacheLoad(cache, line, s, E, b, 0, countHit, countMiss, countEviction);
    (*countHit)++;
    switch (load) {
    case 1:
        if (verbose == 1)
            printf("%s hit hit\n", line);
        break;
    case 2:
        if (verbose == 1)
            printf("%s miss hit\n", line);
        break;
    case 3:
        if (verbose == 1)
            printf("%s miss eviction hit\n", line);
        break;
    default:
        break;
    }
    return 0;
}

int setPara(char **argv, int i, int *s, int *E, int *b, char **file) {
    if (strcmp(argv[i++], "-s") != 0)
        return 1;
    *s = atoi(argv[i++]);
    if (strcmp(argv[i++], "-E") != 0)
        return 1;
    *E = atoi(argv[i++]);
    if (strcmp(argv[i++], "-b") != 0)
        return 1;
    *b = atoi(argv[i++]);
    if (strcmp(argv[i++], "-t") != 0)
        return 1;
    *file = argv[i++];
    return 0;
}

/*
 * 输出帮助信息
 */
void printHelp() {
    printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>");
    printf("Options:");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n\n");
    printf("Examples:\n");
    printf("  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}
