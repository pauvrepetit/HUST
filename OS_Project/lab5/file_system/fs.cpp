#include "fs.h"

QString originText;

int format(FILE *fp) {
    // 格式化
    // 需要进行的操作有
    // 1. 创建superBlock
    // 2. 把位示图全部置为0

    struct SuperBlock super;
    super.blockSize = BLOCK_SIZE; // 一个块4KB
    fseek(fp, 0, SEEK_END); // 定位到文件尾
    super.blockNumber = ftell(fp) / super.blockSize;    // ftell获取文件流中指针的当前值,此处就是文件的字节长度

    super.root.type = 1;    // 1代表目录 0代表普通文件
    super.root.size = 0;
    super.root.valid = 1;
    memset(super.root.filename, 0, sizeof(super.root.filename));
    strcpy(super.root.filename, "/");
    for(int i = 0; i < 10; i++) {
        super.root.dataPoint[i] = 0;
    }
    super.root.blockPoint = 0;

    fseek(fp, 0, SEEK_SET); // 定位到文件头
    fwrite(&super, sizeof(super), 1, fp);
    // 完成superBlock的创建

    fseek(fp, BLOCK_SIZE, SEEK_SET);   // 移动文件指针到第1个块的位置，从该位置开始写入位示图
    char *zero = (char *)malloc(sizeof(char) * BLOCK_SIZE * 16);
    memset(zero, 0, sizeof(char) * BLOCK_SIZE * 16);
    fwrite(zero, sizeof(char), BLOCK_SIZE * 16, fp);
    free(zero);

    fseek(fp, 0, SEEK_SET);
    return 0;
}

int createFile(const char *filePath, FILE *fp) {
    if(filePath[0] != '/') {
        return -1;
    }
    if(filePath[0] == '/' && filePath[1] == '/') {
        return -2;
    }

    char *file = (char *)malloc(sizeof(char) * strlen(filePath) + 1);
    strcpy(file, filePath);
    char *path = strtok(file, "/");

    struct SuperBlock super;
    fseek(fp, 0, SEEK_SET);
    fread(&super, sizeof(super), 1, fp);    // 读super块 获取根目录信息
    struct FileNode *root = &(super.root);

    void *block = malloc(BLOCK_SIZE);
    void *blockNode = malloc(BLOCK_SIZE);
    struct FileNode *nodePointer = (struct FileNode *)block;
    struct FileNode *targetNode;
    long targetOffset = 0;
    char localPath[1024];
    int flag = 0;
    int fileExist = 0;
    int createSuccess = 0;

    while(path != NULL) {
        // 此时path为两个‘/’之间的字符串
        for(int i = 0; i < 10; i++) {
            if(root->dataPoint[i] != 0) {
                // 表示该指针指向一个块
                fseek(fp, BLOCK_SIZE * root->dataPoint[i], SEEK_SET);
                fread(block, BLOCK_SIZE, 1, fp);   // 读出这个块
                for(int j = 0; j < 16; j++) {
                    // 遍历这个块中的16个FileNode
                    if(nodePointer[j].valid == 1 && nodePointer[j].type == 1 && strcmp(nodePointer[j].filename, path) == 0) {
                        // 找到该FileNode
                        flag = 1;
                        targetNode = nodePointer + j;
                        targetOffset = j * 256 + BLOCK_SIZE * root->dataPoint[i];
                        break;
                    }
                    if(nodePointer[j].valid == 1 && nodePointer[j].type == 0 && strcmp(nodePointer[j].filename, path) == 0) {
                        // 找到了一个和path同名的文件，而不是目录，这种情况下，其实是执行不下去的
                        // 有两种情况，第一就是输入的路径有问题
                        // 第二就是当前的path已经是最终的文件名了，那么此时，待创建的文件是已经存在的
                        fileExist = 1;
                        break;
                    }
                }
                if(flag == 1 || fileExist == 1) {
                    break;
                }
            }
        }
        if(flag == 0 && fileExist == 0 && root->blockPoint != 0) {
            // 上面的循环 循环10次是遍历一级索引，下面我们遍历二级索引
            fseek(fp, BLOCK_SIZE * root->blockPoint, SEEK_SET);
            fread(blockNode, BLOCK_SIZE, 1, fp);
            for(int i = 0; i < 512; i++) {
                if(((long *)blockNode)[i] != 0) {
                    fseek(fp, BLOCK_SIZE * ((long *)blockNode)[i], SEEK_SET);
                    fread(block, BLOCK_SIZE, 1, fp);
                    for(int j = 0; j < 16; j++) {
                        if(nodePointer[j].valid == 1 && nodePointer[j].type == 1 && strcmp(nodePointer[j].filename, path) == 0) {
                            // 找到该FileNode
                            flag = 1;
                            targetNode = nodePointer + j;
                            targetOffset = j * 256 + BLOCK_SIZE * ((long *)blockNode)[i];
                            break;
                        }
                        if(nodePointer[j].valid == 1 && nodePointer[j].type == 0 && strcmp(nodePointer[j].filename, path) == 0) {
                            // 找到了一个和path同名的文件，而不是目录，这种情况下，其实是执行不下去的
                            // 有两种情况，第一就是输入的路径有问题
                            // 第二就是当前的path已经是最终的文件名了，那么此时，待创建的文件是已经存在的
                            fileExist = 1;
                            break;
                        }
                    }
                    if(flag == 1 || fileExist == 1) {
                        break;
                    }
                }
            }
        }

        if(flag == 0) {
            strcpy(localPath, path);
            path = strtok(NULL, "/");
            if(path == NULL && fileExist == 1) {
                // 表明 上面遍历的循环是由于fileExist退出的，并且此时的path是最后一个字符串了，即待创建的文件是存在的
                free(file);
                free(block);
                free(blockNode);
                return -5;
            }
            if(fileExist == 1) {
                // 表明 是中间的某一级目录不存在，并且在该目录应该存在的位置存在一个与目录重名的文件
                free(file);
                free(block);
                free(blockNode);
                return -6;
            }
            if(path == NULL) {
                // 表明当前是最后一个path中的字符串了 即待创建的文件名 该文件名为localPath，应该在root这个目录下面
                // 经过上面的if语句，我们可以确定，在root这个目录下面原本不存在path这个文件
                // 我们在这里创建这个文件，此处是正常返回的出口

                for(int i = 0; i < 10; i++) {
                    if(root->dataPoint[i] == 0) {
                        root->dataPoint[i] = getBlock(fp); // 分配一个块 返回该块的块号 我们直接把数据写到这个块里面
                        fseek(fp, targetOffset, SEEK_SET);
                        fwrite(root, sizeof(struct FileNode), 1, fp);
                        // 应该写root所在的块
                        fseek(fp, BLOCK_SIZE * root->dataPoint[i], SEEK_SET);
                        memset(block, 0, BLOCK_SIZE);
                        nodePointer[0].size = 0;
                        nodePointer[0].type = 0;
                        nodePointer[0].valid = 1;
                        strcpy(nodePointer[0].filename, localPath);
                        // 在分配块的时候会全部赋初值0,下面的11个指针全部是0
                        fwrite(block, BLOCK_SIZE, 1, fp);
                        createSuccess = 1;
                        break;
                    } else {
                        // 这个块存在，我们遍历这个块的16个FileNode,如果有无效的(valid == 0),就写入到这个里面去
                        fseek(fp, BLOCK_SIZE * root->dataPoint[i], SEEK_SET);
                        fread(block, BLOCK_SIZE, 1, fp);
                        for(int j = 0; j < 16; j++) {
                            if(nodePointer[j].valid == 0) {
                                // 找到一个空的FileNode的位置，把待创建的文件写进去
                                memset(&nodePointer[j], 0, sizeof(struct FileNode));
                                nodePointer[j].valid = 1;
                                strcpy(nodePointer[j].filename, localPath);
                                createSuccess = 1;
                                break;
                            }
                        }
                        if(createSuccess == 1) {
                            fseek(fp, BLOCK_SIZE * root->dataPoint[i], SEEK_SET);
                            fwrite(block, BLOCK_SIZE, 1, fp);
                            break;
                        }
                    }
                }
                free(file);
                free(block);
                free(blockNode);
                if(createSuccess == 1) {
                    return 0;   // 创建文件成功
                } else {
                    return -7;  // 超过了单个目录下所能够支持的最大的文件数量，创建失败
                }
            } else {
                // 表明，该路径中存在某一级目录是不存在的，此时创建失败，我们要退出
                free(file);
                free(block);
                free(blockNode);
                return -3;
            }
        } else {
            // 表示我们找到目录path了
            memcpy(root, targetNode, sizeof(struct FileNode));      // 更新root
            path = strtok(NULL, "/");
        }
        flag = 0;
    }

    free(file);
    free(block);
    free(blockNode);
    // 出错，我们写的那个路径是一个已经存在的目录的路径
    return -4;
}

int createDir(const char *dirPath, FILE *fp) {
    // 其实创建文件和创建目录的工作是类似的
    if(dirPath[0] != '/') {
        return -1;
    }
    if(dirPath[0] == '/' && dirPath[1] == '/') {
        return -2;
    }

    char *dir = (char *)malloc(sizeof(char) * strlen(dirPath) + 1);
    strcpy(dir, dirPath);
    char *path = strtok(dir, "/");

    struct SuperBlock super;
    fseek(fp, 0, SEEK_SET);
    fread(&super, sizeof(super), 1, fp);    // 读super块 获取根目录信息
    struct FileNode *root = &(super.root);

    void *block = malloc(BLOCK_SIZE);
    void *blockNode = malloc(BLOCK_SIZE);
    struct FileNode *nodePointer = (struct FileNode *)block;
    struct FileNode *targetNode;
    long targetOffset = 0;
    char localPath[1024];
    int flag = 0;
    int fileExist = 0;
    int createSuccess = 0;

    while(path != NULL) {
        // 此时path为两个‘/’之间的字符串
        for(int i = 0; i < 10; i++) {
            if(root->dataPoint[i] != 0) {
                // 表示该指针指向一个块
                fseek(fp, BLOCK_SIZE * root->dataPoint[i], SEEK_SET);
                fread(block, BLOCK_SIZE, 1, fp);   // 读出这个块
                for(int j = 0; j < 16; j++) {
                    // 遍历这个块中的16个FileNode
                    if(nodePointer[j].valid == 1 && nodePointer[j].type == 1 && strcmp(nodePointer[j].filename, path) == 0) {
                        // 找到该FileNode
                        flag = 1;
                        targetNode = nodePointer + j;
                        targetOffset = j * 256 + BLOCK_SIZE * root->dataPoint[i];
                        break;
                    }
                    if(nodePointer[j].valid == 1 && nodePointer[j].type == 0 && strcmp(nodePointer[j].filename, path) == 0) {
                        // 找到了一个和path同名的文件，而不是目录，这种情况下，其实是执行不下去的
                        // 有两种情况，第一就是输入的路径有问题
                        // 第二就是当前的path已经是最终的目录名了
                        fileExist = 1;
                        break;
                    }
                }
                if(flag == 1 || fileExist == 1) {
                    break;
                }
            }
        }
        if(flag == 0 && fileExist == 0 && root->blockPoint != 0) {
            // root->blockPoint == 0表示二级索引不存在，就不用搜索了
            // 上面的循环 循环10次是遍历一级索引，下面我们遍历二级索引
            fseek(fp, BLOCK_SIZE * root->blockPoint, SEEK_SET);
            fread(blockNode, BLOCK_SIZE, 1, fp);
            for(int i = 0; i < 512; i++) {
                if(((long *)blockNode)[i] != 0) {
                    fseek(fp, BLOCK_SIZE * ((long *)blockNode)[i], SEEK_SET);
                    fread(block, BLOCK_SIZE, 1, fp);
                    for(int j = 0; j < 16; j++) {
                        if(nodePointer[j].valid == 1 && nodePointer[j].type == 1 && strcmp(nodePointer[j].filename, path) == 0) {
                            // 找到该FileNode
                            flag = 1;
                            targetNode = nodePointer + j;
                            targetOffset = j * 256 + BLOCK_SIZE * ((long *)blockNode)[i];
                            break;
                        }
                        if(nodePointer[j].valid == 1 && nodePointer[j].type == 0 && strcmp(nodePointer[j].filename, path) == 0) {
                            // 找到了一个和path同名的文件，而不是目录，这种情况下，其实是执行不下去的
                            // 有两种情况，第一就是输入的路径有问题
                            // 第二就是当前的path已经是最终的目录名了，那么此时，也是有问题的
                            fileExist = 1;
                            break;
                        }
                    }
                    if(flag == 1 || fileExist == 1) {
                        break;
                    }
                }
            }
        }

        if(flag == 0) {
            // 没有找到当前的目录的
            strcpy(localPath, path);
            path = strtok(NULL, "/");
            if(path == NULL && fileExist == 1) {
                // 表明 上面遍历的循环是由于fileExist退出的，并且此时的path是最后一个字符串了，即待创建的目录存在一个同名的文件，创建失败
                free(dir);
                free(block);
                free(blockNode);
                return -5;
            }
            if(fileExist == 1) {
                // 表明 是中间的某一级目录不存在，并且在该目录应该存在的位置存在一个与目录重名的文件
                free(dir);
                free(block);
                free(blockNode);
                return -6;
            }
            if(path == NULL) {
                // 表明当前是最后一个path中的字符串了 即待创建的目录名 该目录名为localPath，应该在root(我们每次循环都会更新的)这个目录下面
                // 经过上面的if语句，我们可以确定，在root这个目录下面原本不存在path这个文件/目录的
                // 我们在这里创建这个目录，此处是正常返回的出口

                for(int i = 0; i < 10; i++) {
                    if(root->dataPoint[i] == 0) {
                        root->dataPoint[i] = getBlock(fp); // 分配一个块 返回该块的块号 我们直接把数据写到这个块里面
                        fseek(fp, targetOffset, SEEK_SET);
                        fwrite(root, sizeof(struct FileNode), 1, fp);
                        // 应该写root所在的块
                        fseek(fp, BLOCK_SIZE * root->dataPoint[i], SEEK_SET);
                        memset(block, 0, BLOCK_SIZE);
                        nodePointer[0].size = 0;
                        nodePointer[0].type = 1;    // 表明是目录
                        nodePointer[0].valid = 1;
                        strcpy(nodePointer[0].filename, localPath);
                        // 在分配块的时候会全部赋初值0,下面的11个指针全部是0
                        fwrite(block, BLOCK_SIZE, 1, fp);
                        createSuccess = 1;
                        break;
                    } else {
                        // 这个块存在，我们遍历这个块的16个FileNode,如果有无效的(valid == 0),就写入到这个里面去
                        fseek(fp, BLOCK_SIZE * root->dataPoint[i], SEEK_SET);
                        fread(block, BLOCK_SIZE, 1, fp);
                        for(int j = 0; j < 16; j++) {
                            if(nodePointer[j].valid == 0) {
                                // 找到一个空的FileNode的位置，把待创建的文件写进去
                                memset(&nodePointer[j], 0, sizeof(struct FileNode));
                                nodePointer[j].valid = 1;
                                nodePointer[j].type = 1;
                                strcpy(nodePointer[j].filename, localPath);
                                createSuccess = 1;
                                break;
                            }
                        }
                        if(createSuccess == 1) {
                            fseek(fp, BLOCK_SIZE * root->dataPoint[i], SEEK_SET);
                            fwrite(block, BLOCK_SIZE, 1, fp);
                            break;
                        }
                    }
                }
                free(dir);
                free(block);
                free(blockNode);
                if(createSuccess == 1) {
                    return 0;   // 创建成功
                } else {
                    return -7;  // 超过了单个目录下所能够支持的最大的文件数量，创建失败
                }
            } else {
                // 表明，该路径中存在某一级目录是不存在的，此时创建失败，我们要退出
                free(dir);
                free(block);
                free(blockNode);
                return -3;
            }
        } else {
            // 表示我们找到目录path了
            memcpy(root, targetNode, sizeof(struct FileNode));      // 更新root
            path = strtok(NULL, "/");
        }
        flag = 0;
    }

    free(dir);
    free(block);
    free(blockNode);
    // 出错，我们写的那个路径是一个已经存在的目录的路径
    return -4;
}

long openFile(const char *filePath, FILE *fp, Ui::MainWindow *ui) {
    if(filePath[0] != '/') {
        return -1;
    }
    if(filePath[0] == '/' && filePath[1] == '/') {
        return -2;
    }

    char *file = (char *)malloc(sizeof(char) * strlen(filePath) + 1);
    strcpy(file, filePath);
    char *path = strtok(file, "/");

    struct SuperBlock super;
    fseek(fp, 0, SEEK_SET);
    fread(&super, sizeof(super), 1, fp);    // 读super块 获取根目录信息
    struct FileNode *root = &(super.root);

    void *block = malloc(BLOCK_SIZE);
    void *blockNode = malloc(BLOCK_SIZE);
    struct FileNode *nodePointer = (struct FileNode *)block;
    struct FileNode *targetNode;
    long fileNodeOffset = 0;
    char localPath[1024];
    int flag = 0;
    int fileExist = 0;

    while(path != NULL) {
        // 此时path为两个‘/’之间的字符串
        for(int i = 0; i < 10; i++) {
            if(root->dataPoint[i] != 0) {
                // 表示该指针指向一个块
                fseek(fp, BLOCK_SIZE * root->dataPoint[i], SEEK_SET);
                fread(block, BLOCK_SIZE, 1, fp);   // 读出这个块
                for(int j = 0; j < 16; j++) {
                    // 遍历这个块中的16个FileNode
                    if(nodePointer[j].valid == 1 && nodePointer[j].type == 1 && strcmp(nodePointer[j].filename, path) == 0) {
                        // 找到该FileNode
                        flag = 1;
                        targetNode = nodePointer + j;
                        break;
                    }
                    if(nodePointer[j].valid == 1 && nodePointer[j].type == 0 && strcmp(nodePointer[j].filename, path) == 0) {
                        // 找到了一个和path同名的文件，而不是目录
                        // 有两种情况，第一就是输入的路径有问题
                        // 第二就是当前的path已经是最终的文件名了，我们要打开的文件就是这个nodePointer[j](这是一个FileNode)
                        fileExist = 1;
                        nodePointer += j;   // 这样的话，我们的nodePointer就指向我们要打开的文件的FileNode
                        fileNodeOffset = j * 256 + BLOCK_SIZE * root->dataPoint[i];
                        break;
                    }
                }
                if(flag == 1 || fileExist == 1) {
                    break;
                }
            }
        }
        if(flag == 0 && fileExist == 0 && root->blockPoint != 0) {
            // 上面的循环 循环10次是遍历一级索引，下面我们遍历二级索引
            fseek(fp, BLOCK_SIZE * root->blockPoint, SEEK_SET);
            fread(blockNode, BLOCK_SIZE, 1, fp);
            for(int i = 0; i < 512; i++) {
                if(((long *)blockNode)[i] != 0) {
                    fseek(fp, BLOCK_SIZE * ((long *)blockNode)[i], SEEK_SET);
                    fread(block, BLOCK_SIZE, 1, fp);
                    for(int j = 0; j < 16; j++) {
                        if(nodePointer[j].valid == 1 && nodePointer[j].type == 1 && strcmp(nodePointer[j].filename, path) == 0) {
                            // 找到该FileNode
                            flag = 1;
                            targetNode = nodePointer + j;
                            break;
                        }
                        if(nodePointer[j].valid == 1 && nodePointer[j].type == 0 && strcmp(nodePointer[j].filename, path) == 0) {
                            // 找到了一个和path同名的文件，而不是目录
                            // 有两种情况，第一就是输入的路径有问题
                            // 第二就是当前的path已经是最终的文件名了，我们要打开的文件就是这个nodePointer[j]
                            fileExist = 1;
                            nodePointer += j;
                            fileNodeOffset = j * 256 + BLOCK_SIZE * ((long *)blockNode)[i];
                            break;
                        }
                    }
                    if(flag == 1 || fileExist == 1) {
                        break;
                    }
                }
            }
        }

        if(flag == 0) {
            strcpy(localPath, path);
            path = strtok(NULL, "/");
            if(path == NULL && fileExist == 1) {
                // 表明 上面遍历的循环是由于fileExist退出的，并且此时的path是最后一个字符串了，即找到了我们要打开的文件
                // 此处是打开文件 正常退出的出口
                // 此时待打开的文件的FileNode就是nodePointer

                originText = ui->shell->toPlainText();
                ui->shell->clear();
                // 首先保存shell中原来的内容，然后将shell清空 下面读取文件的内容并写入到shell中

                int fileSize = nodePointer->size;
                char contentBuf[BLOCK_SIZE + 1];
                contentBuf[BLOCK_SIZE] = 0;
                int i;
                for(i = 0; i < fileSize / BLOCK_SIZE && i < 10; i++) {
                    fseek(fp, nodePointer->dataPoint[i] * BLOCK_SIZE, SEEK_SET);
                    fread(contentBuf, BLOCK_SIZE, 1, fp);
                    ui->shell->insertPlainText(contentBuf);
                }
                if(i == 10) {
                    // 前面的10个一级索引是用完了的
                    // 当然 有可能是前面的10个一级索引恰好存完了所有的数据，此时 没有使用二级索引
                    // 更多的情况是 使用了二级索引
                    fseek(fp, nodePointer->blockPoint * BLOCK_SIZE, SEEK_SET);
                    fread(blockNode, BLOCK_SIZE, 1, fp);    // 读出二级索引表
                    int j = 0;
                    for(j = 0; j < (fileSize - BLOCK_SIZE * 10) / BLOCK_SIZE; j++) {
                        // 使用的完整的二级索引
                        fseek(fp, ((long *)blockNode)[j] * BLOCK_SIZE, SEEK_SET);
                        fread(contentBuf, BLOCK_SIZE, 1, fp);
                        ui->shell->insertPlainText(contentBuf);
                    }
                    // 接下来是一个没有使用完的二级索引
                    fseek(fp, ((long *)blockNode)[j] * BLOCK_SIZE, SEEK_SET);
                    memset(contentBuf, 0, BLOCK_SIZE);
                    fread(contentBuf, fileSize % BLOCK_SIZE, 1, fp);
                    ui->shell->insertPlainText(contentBuf);
                } else {
                    // 此时是明确的，前面的10个一级索引是没有用完的 我们从下一个一级索引中读取余下的部分 写入到shell中
                    fseek(fp, nodePointer->dataPoint[i] * BLOCK_SIZE, SEEK_SET);
                    memset(contentBuf, 0, BLOCK_SIZE);
                    fread(contentBuf, fileSize % BLOCK_SIZE, 1, fp);
                    ui->shell->insertPlainText(contentBuf);
                }

                free(file);
                free(block);
                free(blockNode);
                return fileNodeOffset;
            }
            if(fileExist == 1) {
                // 表明 是中间的某一级目录不存在，并且在该目录应该存在的位置存在一个与目录重名的文件
                free(file);
                free(block);
                free(blockNode);
                return -6;
            }
            if(path == NULL) {
                // 表明当前是最后一个path中的字符串了
                // 经过上面的if语句，我们可以确定，在root这个目录下面原本不存在path这个文件
                free(file);
                free(block);
                free(blockNode);
                return -7;  // 待打开的文件不存在
            } else {
                // 表明，该路径中存在某一级目录是不存在的
                free(file);
                free(block);
                free(blockNode);
                return -3;
            }
        } else {
            // 表示我们找到目录path了
            memcpy(root, targetNode, sizeof(struct FileNode));      // 更新root
            path = strtok(NULL, "/");
        }
        flag = 0;
    }

    free(file);
    free(block);
    free(blockNode);
    // 出错，我们写的那个路径是一个已经存在的目录的路径
    return -4;
}

int writeClose(const char *fileContent, long fileNodeOffset, FILE *fp) {
    // fileNodeOffset为待写入的文件的FileNode在文件fp中所处的位置的偏移量
    fseek(fp, fileNodeOffset, SEEK_SET);
    struct FileNode file;
    fread(&file, sizeof(struct FileNode), 1, fp);
    int fileLength = strlen(fileContent);
    int originLength = file.size;
    file.size = fileLength;
    int fileBlock = fileLength / BLOCK_SIZE;
    if(fileLength % BLOCK_SIZE != 0) {
        fileBlock++;
    }
    int originBlock = originLength / BLOCK_SIZE;
    if(originLength % BLOCK_SIZE != 0) {
        originBlock++;
    }
    void *blockNode = malloc(BLOCK_SIZE);
    if(fileBlock == originBlock) {
        // 说明 文件内容更改前后所占用的块的数量是一样的
        // 不需要申请新的块或者释放块
        int i;
        for(i = 0; i < (fileBlock - 1) && i < 10; i++) {
            fseek(fp, file.dataPoint[i] * BLOCK_SIZE, SEEK_SET);
            fwrite(fileContent + i * BLOCK_SIZE, BLOCK_SIZE, 1, fp);
        }
        if(i == 10) {
            //表明 10个一级索引都用完了
            fseek(fp, file.blockPoint * BLOCK_SIZE, SEEK_SET);
            fread(blockNode, BLOCK_SIZE, 1, fp);
            // blockNode中保存了二级索引的索引表
            int j = 0;
            for(j = 0; j < fileBlock - 10 - 1; j++) {
                // 写入完整的二级索引的数据
                fseek(fp, ((long *)blockNode)[j] * BLOCK_SIZE, SEEK_SET);
                fwrite(fileContent + (10 + j) * BLOCK_SIZE, BLOCK_SIZE, 1, fp);
            }
            // 剩下一个不完全的二级索引
            fseek(fp, ((long *)blockNode)[j] * BLOCK_SIZE, SEEK_SET);
            fwrite(fileContent + (10 + j) * BLOCK_SIZE, fileLength % BLOCK_SIZE, 1, fp);
        } else {
            // 10个一级索引没有用完 写入最后一个一级索引的数据
            fseek(fp, file.dataPoint[i] * BLOCK_SIZE, SEEK_SET);
            fwrite(fileContent + i * BLOCK_SIZE, fileLength % BLOCK_SIZE, 1, fp);
        }

        free(blockNode);
        fseek(fp, fileNodeOffset, SEEK_SET);
        fwrite(&file, sizeof(struct FileNode), 1, fp);
        return 0;
    } else if(fileBlock > originBlock) {
        // 说明我们需要在原有块的基础上获取新的块 用来存储更多的数据
        int i;
        for(i = 0; i < originBlock && i < 10; i++) {
            fseek(fp, file.dataPoint[i] * BLOCK_SIZE, SEEK_SET);
            fwrite(fileContent + i * BLOCK_SIZE, BLOCK_SIZE, 1, fp);
        }
        if(i == 10) {
            fseek(fp, file.blockPoint * BLOCK_SIZE, SEEK_SET);
            fread(blockNode, BLOCK_SIZE, 1, fp);
            int j = 0;
            for(j = 0; j < originBlock - 10; j++) {
                fseek(fp, ((long *)blockNode)[j] * BLOCK_SIZE, SEEK_SET);
                fwrite(fileContent + (10 + j) * BLOCK_SIZE, BLOCK_SIZE, 1, fp);
                // 将原有的块全部使用完
            }
            // 接下来我们要添加新的块用于存储余下的数据
            // 这里的状况是，以及有超过10个块了，所以直接在二级索引里面添加新的块
            for(j = 0; j < fileBlock - originBlock - 1; j++) {
                ((long *)blockNode)[j + originBlock - 10] = getBlock(fp);       // warning: 这个blockNode的块最后要写入到文件系统中
                fseek(fp, ((long *)blockNode)[j + originBlock - 10] * BLOCK_SIZE, SEEK_SET);
                fwrite(fileContent + (originBlock + j) * BLOCK_SIZE, BLOCK_SIZE, 1, fp);
            }
            // 最后一个块 可能是填不满的
            ((long *)blockNode)[j + originBlock - 10] = getBlock(fp);
            fseek(fp, ((long *)blockNode)[j + originBlock - 10] * BLOCK_SIZE, SEEK_SET);
            char temp[BLOCK_SIZE];
            memset(temp, 0, BLOCK_SIZE);
            fwrite(temp, BLOCK_SIZE, 1, fp);
            fseek(fp, ((long *)blockNode)[j + originBlock - 10] * BLOCK_SIZE, SEEK_SET);
            fwrite(fileContent + (fileBlock - 1) * BLOCK_SIZE, fileLength - (fileBlock - 1) * BLOCK_SIZE, 1, fp);

            // 这个分支是 需要添加新的块 且原来的块就已经填满一级索引的情况
            fseek(fp, fileNodeOffset, SEEK_SET);
            fwrite(&file, sizeof(struct FileNode), 1, fp);
            fseek(fp, file.blockPoint * BLOCK_SIZE, SEEK_SET);
            fwrite(blockNode, BLOCK_SIZE, 1, fp);
            free(blockNode);
            return 0;
        } else {
            // 原有的块 没有填满一级索引 我们要首先填满一级索引 如果仍然不够，那么我们再填二级索引
            if(fileBlock <= 10) {
                // 只需要使用一级索引 不需要使用二级索引
                int j = originBlock;
                for(j = originBlock; j < fileBlock - 1; j++) {
                    file.dataPoint[j] = getBlock(fp);
                    fseek(fp, file.dataPoint[j] * BLOCK_SIZE, SEEK_SET);
                    fwrite(fileContent + j * BLOCK_SIZE, BLOCK_SIZE, 1, fp);
                }
                file.dataPoint[j] = getBlock(fp);
                fseek(fp, file.dataPoint[j] * BLOCK_SIZE, SEEK_SET);
                char temp[BLOCK_SIZE];
                memset(temp, 0, BLOCK_SIZE);
                fwrite(temp, BLOCK_SIZE, 1, fp);
                fseek(fp, file.dataPoint[j] * BLOCK_SIZE, SEEK_SET);
                fwrite(fileContent + j * BLOCK_SIZE, fileLength - j * BLOCK_SIZE, 1, fp);
            } else {
                // 此时需要使用到二级索引，首先把一级索引用完，然后剩下的部分使用二级索引
                int j = originBlock;
                for(j = originBlock; j < 10; j++) {
                    file.dataPoint[j] = getBlock(fp);
                    fseek(fp, file.dataPoint[j] * BLOCK_SIZE, SEEK_SET);
                    fwrite(fileContent + j * BLOCK_SIZE, BLOCK_SIZE, 1, fp);
                }
                // 上面把一级索引用完了，下面使用二级索引
                // 首先获取一个块 用于存储二级索引的索引表
                file.blockPoint = getBlock(fp); // warning: 这个块 我们最后是要把数据写入的文件系统的
                memset(blockNode, 0, BLOCK_SIZE);
                for(j = 0; j < fileBlock - 1 - 10; j++) {
                    ((long *)blockNode)[j] = getBlock(fp);
                    fseek(fp, ((long *)blockNode)[j] * BLOCK_SIZE, SEEK_SET);
                    fwrite(fileContent + (j + 10) * BLOCK_SIZE, BLOCK_SIZE, 1, fp);
                }
                ((long *)blockNode)[j] = getBlock(fp);
                fseek(fp, ((long *)blockNode)[j] * BLOCK_SIZE, SEEK_SET);
                char temp[BLOCK_SIZE];
                memset(temp, 0, BLOCK_SIZE);
                fwrite(temp, BLOCK_SIZE, 1, fp);
                fseek(fp, ((long *)blockNode)[j] * BLOCK_SIZE, SEEK_SET);
                fwrite(fileContent + (j + 10) * BLOCK_SIZE, fileLength - (j + 10) * BLOCK_SIZE, 1, fp);

                // 这里要先把二级索引的索引表写回到文件系统
                fseek(fp, file.blockPoint * BLOCK_SIZE, SEEK_SET);
                fwrite(blockNode, BLOCK_SIZE, 1, fp);
            }

            fseek(fp, fileNodeOffset, SEEK_SET);
            fwrite(&file, sizeof(struct FileNode), 1, fp);
            free(blockNode);
            return 0;
        }
    } else {
        // 这里，原有的文件比修改后的文件所需要的块要多，我们需要释放掉多余的块
        int i;
        for(i = 0; i < (fileBlock - 1) && i < 10; i++) {
            fseek(fp, file.dataPoint[i] * BLOCK_SIZE, SEEK_SET);
            fwrite(fileContent + i * BLOCK_SIZE, BLOCK_SIZE, 1, fp);
        }
        // 首先写一级索引指向的内容
        if(fileBlock <= 10) {
            // 说明现在已经写完了，然后二级索引全部没有用，我们把余下的一级索引和二级索引释放掉
            fseek(fp, file.dataPoint[fileBlock - 1] * BLOCK_SIZE, SEEK_SET);
            fwrite(fileContent + i * BLOCK_SIZE, fileLength % BLOCK_SIZE, 1, fp);
            // 首先把最后的不到一个块的内容写入到文件系统中
            int j;
            if(originBlock <= 10) {
                for(j = fileBlock; j < originBlock; j++) {
                    releaseBlock(file.dataPoint[j], fp);
                    file.dataPoint[j] = 0;
                }
            } else {
                for(j = fileBlock; j < 10; j++) {
                    releaseBlock(file.dataPoint[j], fp);
                    file.dataPoint[j] = 0;
                }
                fseek(fp, file.blockPoint * BLOCK_SIZE, SEEK_SET);
                fread(blockNode, BLOCK_SIZE, 1, fp);
                for(j = 0; j < originBlock - 10; j++) {
                    releaseBlock(((long *)blockNode)[j], fp);
                    ((long *)blockNode)[j] = 0;
                }
                releaseBlock(file.blockPoint, fp);
                file.blockPoint = 0;
            }
        } else {
            // 不然的话 二级索引是用过的 那么需要接着写二级索引 然后把余下的二级所以释放掉
            fseek(fp, file.blockPoint * BLOCK_SIZE, SEEK_SET);
            fread(blockNode, BLOCK_SIZE, 1, fp);
            int j = 0;
            for(j = 0; j < fileBlock - 10 - 1; j++) {
                fseek(fp, ((long *)blockNode)[j] * BLOCK_SIZE, SEEK_SET);
                fwrite(fileContent + (10 + j) * BLOCK_SIZE, BLOCK_SIZE, 1, fp);
            }
            fseek(fp, ((long *)blockNode)[j] * BLOCK_SIZE, SEEK_SET);
            fwrite(fileContent + (10 + j) * BLOCK_SIZE, fileLength % BLOCK_SIZE, 1, fp);

            for(; j < originBlock - 10; j++) {
                releaseBlock(((long *)blockNode)[j], fp);
                ((long *)blockNode)[j] = 0;
            }

            // 这里需要把blockNode写回文件系统中
            fseek(fp, file.blockPoint * BLOCK_SIZE, SEEK_SET);
            fwrite(blockNode, BLOCK_SIZE, 1, fp);
        }
        // 我们在这里把file写回
        fseek(fp, fileNodeOffset, SEEK_SET);
        fwrite(&file, sizeof(struct FileNode), 1, fp);
        return 0;
    }
}

int listFile(const char *dirPath, FILE *fp, Ui::MainWindow *ui) {
    if(dirPath[0] != '/') {
        return -1;
    }
    if(dirPath[0] == '/' && dirPath[1] == '/') {
        return -2;
    }

    char *dir = (char *)malloc(sizeof(char) * strlen(dirPath) + 1);
    strcpy(dir, dirPath);
    char *path = strtok(dir, "/");

    struct SuperBlock super;
    fseek(fp, 0, SEEK_SET);
    fread(&super, sizeof(super), 1, fp);    // 读super块 获取根目录信息
    struct FileNode *root = &(super.root);

    void *block = malloc(BLOCK_SIZE);
    void *blockNode = malloc(BLOCK_SIZE);
    struct FileNode *nodePointer = (struct FileNode *)block;
    struct FileNode *targetNode;
    int flag = 0;
    int fileExist = 0;

    while(path != NULL) {
        // 此时path为两个‘/’之间的字符串
        for(int i = 0; i < 10; i++) {
            if(root->dataPoint[i] != 0) {
                // 表示该指针指向一个块
                fseek(fp, BLOCK_SIZE * root->dataPoint[i], SEEK_SET);
                fread(block, BLOCK_SIZE, 1, fp);   // 读出这个块
                for(int j = 0; j < 16; j++) {
                    // 遍历这个块中的16个FileNode
                    if(nodePointer[j].valid == 1 && nodePointer[j].type == 1 && strcmp(nodePointer[j].filename, path) == 0) {
                        // 找到该FileNode
                        flag = 1;
                        targetNode = nodePointer + j;
                        break;
                    }
                    if(nodePointer[j].valid == 1 && nodePointer[j].type == 0 && strcmp(nodePointer[j].filename, path) == 0) {
                        // 找到了一个和path同名的文件,此时我们认为是出错的，path应该为一个目录，无论其是不是dirPath中的最后一部分
                        fileExist = 1;
                        break;
                    }
                }
                if(flag == 1 || fileExist == 1) {
                    break;
                }
            }
        }

        // 我们在创建文件/目录的时候没有使用二级索引，所以一个目录下面最多只能有10 × 16个 文件/目录

        if(flag == 0) {
            // flag == 0 表示没有找到那个文件 此时是必然出错的 直接返回
            free(dir);
            free(block);
            free(blockNode);
            return -5;
        } else {
            // 表示我们找到目录path了
            memcpy(root, targetNode, sizeof(struct FileNode));      // 更新root
            path = strtok(NULL, "/");
        }
        flag = 0;
    }

    QString showFile;
    for(int i = 0; i < 10; i++) {
        if(root->dataPoint[i] == 0) {
            break;
        }
        fseek(fp, root->dataPoint[i] * BLOCK_SIZE, SEEK_SET);
        fread(block, BLOCK_SIZE, 1, fp);
        for(int j = 0; j < 16; j++) {
            if(nodePointer[j].valid == 1) {
                if(nodePointer[j].type == 0) {
                    // 找到一个文件
                    showFile = QString(nodePointer[j].filename) + "\tRegular File\t" + QString::number(nodePointer[j].size) + "Bytes\n";
                    ui->shell->insertPlainText(showFile);
                } else {
                    showFile = QString(nodePointer[j].filename) + "\tDirectory\n";
                    ui->shell->insertPlainText(showFile);
                    // 找到一个目录
                }
            }
        }
    }

    free(dir);
    free(block);
    free(blockNode);
    // 此处为正确的出口
    return 0;
}

int rmFile(const char *filePath, FILE *fp) {
    // 删除文件首先需要找到文件 我们在openFile的基础上进行修改 只需要将原来的打开的操作改为删除的操作即可
    if(filePath[0] != '/') {
        return -1;
    }
    if(filePath[0] == '/' && filePath[1] == '/') {
        return -2;
    }

    char *file = (char *)malloc(sizeof(char) * strlen(filePath) + 1);
    strcpy(file, filePath);
    char *path = strtok(file, "/");

    struct SuperBlock super;
    fseek(fp, 0, SEEK_SET);
    fread(&super, sizeof(super), 1, fp);    // 读super块 获取根目录信息
    struct FileNode *root = &(super.root);

    void *block = malloc(BLOCK_SIZE);
    void *blockNode = malloc(BLOCK_SIZE);
    struct FileNode *nodePointer = (struct FileNode *)block;
    struct FileNode *targetNode;
    long fileNodeOffset = 0;
    char localPath[1024];
    int flag = 0;
    int fileExist = 0;

    while(path != NULL) {
        // 此时path为两个‘/’之间的字符串
        for(int i = 0; i < 10; i++) {
            if(root->dataPoint[i] != 0) {
                // 表示该指针指向一个块
                fseek(fp, BLOCK_SIZE * root->dataPoint[i], SEEK_SET);
                fread(block, BLOCK_SIZE, 1, fp);   // 读出这个块
                for(int j = 0; j < 16; j++) {
                    // 遍历这个块中的16个FileNode
                    if(nodePointer[j].valid == 1 && nodePointer[j].type == 1 && strcmp(nodePointer[j].filename, path) == 0) {
                        // 找到该FileNode
                        flag = 1;
                        targetNode = nodePointer + j;
                        break;
                    }
                    if(nodePointer[j].valid == 1 && nodePointer[j].type == 0 && strcmp(nodePointer[j].filename, path) == 0) {
                        // 找到了一个和path同名的文件，而不是目录
                        // 有两种情况，第一就是输入的路径有问题
                        // 第二就是当前的path已经是最终的文件名了，我们要打开的文件就是这个nodePointer[j](这是一个FileNode)
                        fileExist = 1;
                        nodePointer += j;   // 这样的话，我们的nodePointer就指向我们要打开的文件的FileNode
                        fileNodeOffset = j * 256 + BLOCK_SIZE * root->dataPoint[i];
                        break;
                    }
                }
                if(flag == 1 || fileExist == 1) {
                    break;
                }
            }
        }
        if(flag == 0 && fileExist == 0 && root->blockPoint != 0) {
            // 上面的循环 循环10次是遍历一级索引，下面我们遍历二级索引
            fseek(fp, BLOCK_SIZE * root->blockPoint, SEEK_SET);
            fread(blockNode, BLOCK_SIZE, 1, fp);
            for(int i = 0; i < 512; i++) {
                if(((long *)blockNode)[i] != 0) {
                    fseek(fp, BLOCK_SIZE * ((long *)blockNode)[i], SEEK_SET);
                    fread(block, BLOCK_SIZE, 1, fp);
                    for(int j = 0; j < 16; j++) {
                        if(nodePointer[j].valid == 1 && nodePointer[j].type == 1 && strcmp(nodePointer[j].filename, path) == 0) {
                            // 找到该FileNode
                            flag = 1;
                            targetNode = nodePointer + j;
                            break;
                        }
                        if(nodePointer[j].valid == 1 && nodePointer[j].type == 0 && strcmp(nodePointer[j].filename, path) == 0) {
                            // 找到了一个和path同名的文件，而不是目录
                            // 有两种情况，第一就是输入的路径有问题
                            // 第二就是当前的path已经是最终的文件名了，我们要打开的文件就是这个nodePointer[j]
                            fileExist = 1;
                            nodePointer += j;
                            fileNodeOffset = j * 256 + BLOCK_SIZE * ((long *)blockNode)[i];
                            break;
                        }
                    }
                    if(flag == 1 || fileExist == 1) {
                        break;
                    }
                }
            }
        }

        if(flag == 0) {
            strcpy(localPath, path);
            path = strtok(NULL, "/");
            if(path == NULL && fileExist == 1) {
                // 表明 上面遍历的循环是由于fileExist退出的，并且此时的path是最后一个字符串了，即找到了我们要打开的文件
                // 此处是正常退出的出口
                // 此时待删除的文件的FileNode就是nodePointer

                // 此时的文件所在的目录为root

                for(int i = 0; i < 10; i++) {
                    if(nodePointer->dataPoint[i] != 0) {
                        releaseBlock(nodePointer->dataPoint[i], fp);
                        nodePointer->dataPoint[i] = 0;
                    }
                }
                if(nodePointer->blockPoint != 0) {
                    fseek(fp, nodePointer->blockPoint * BLOCK_SIZE, SEEK_SET);
                    fread(blockNode, BLOCK_SIZE, 1, fp);
                    for(int i = 0; i < 256; i++) {
                        if(((long *)blockNode)[i] != 0) {
                            releaseBlock(((long *)blockNode)[i], fp);
                            ((long *)blockNode)[i] = 0;
                        }
                    }
                    releaseBlock(nodePointer->blockPoint, fp);
                    nodePointer->blockPoint = 0;
                }
                memset(nodePointer, 0, sizeof(struct FileNode));
                fseek(fp, fileNodeOffset, SEEK_SET);
                fwrite(nodePointer, sizeof(struct FileNode), 1, fp);

                free(file);
                free(block);
                free(blockNode);
                return 0;
            }
            if(fileExist == 1) {
                // 表明 是中间的某一级目录不存在，并且在该目录应该存在的位置存在一个与目录重名的文件
                free(file);
                free(block);
                free(blockNode);
                return -6;
            }
            if(path == NULL) {
                // 表明当前是最后一个path中的字符串了
                // 经过上面的if语句，我们可以确定，在root这个目录下面原本不存在path这个文件
                free(file);
                free(block);
                free(blockNode);
                return -7;  // 待打开的文件不存在
            } else {
                // 表明，该路径中存在某一级目录是不存在的
                free(file);
                free(block);
                free(blockNode);
                return -3;
            }
        } else {
            // 表示我们找到目录path了
            memcpy(root, targetNode, sizeof(struct FileNode));      // 更新root
            path = strtok(NULL, "/");
        }
        flag = 0;
    }

    free(file);
    free(block);
    free(blockNode);
    // 出错，我们写的那个路径是一个已经存在的目录的路径
    return -4;
}

int rmDir(const char *dirPath, FILE *fp) {
    // 删除目录首先要找到这个目录 找目录和找文件其实是一样的 只是最后的判断不同
    if(dirPath[0] != '/') {
        return -1;
    }
    if(dirPath[0] == '/' && dirPath[1] == '/') {
        return -2;
    }

    char *dir = (char *)malloc(sizeof(char) * strlen(dirPath) + 1);
    strcpy(dir, dirPath);
    char *path = strtok(dir, "/");

    struct SuperBlock super;
    fseek(fp, 0, SEEK_SET);
    fread(&super, sizeof(super), 1, fp);    // 读super块 获取根目录信息
    struct FileNode *root = &(super.root);

    void *block = malloc(BLOCK_SIZE);
    void *blockNode = malloc(BLOCK_SIZE);
    struct FileNode *nodePointer = (struct FileNode *)block;
    struct FileNode *targetNode;
    long fileNodeOffset = 0;
    long targetOffset = 0;
    char localPath[1024];
    int flag = 0;
    int fileExist = 0;

    while(path != NULL) {
        // 此时path为两个‘/’之间的字符串
        for(int i = 0; i < 10; i++) {
            if(root->dataPoint[i] != 0) {
                // 表示该指针指向一个块
                fseek(fp, BLOCK_SIZE * root->dataPoint[i], SEEK_SET);
                fread(block, BLOCK_SIZE, 1, fp);   // 读出这个块
                for(int j = 0; j < 16; j++) {
                    // 遍历这个块中的16个FileNode
                    if(nodePointer[j].valid == 1 && nodePointer[j].type == 1 && strcmp(nodePointer[j].filename, path) == 0) {
                        // 找到该FileNode
                        flag = 1;
                        targetNode = nodePointer + j;
                        targetOffset = j * 256 + BLOCK_SIZE * root->dataPoint[i];
                        break;
                    }
                    if(nodePointer[j].valid == 1 && nodePointer[j].type == 0 && strcmp(nodePointer[j].filename, path) == 0) {
                        // 找到了一个和path同名的文件，而不是目录
                        // 有两种情况，第一就是输入的路径有问题
                        // 第二就是当前的path已经是最终的文件名了，我们要打开的文件就是这个nodePointer[j](这是一个FileNode)
                        fileExist = 1;
                        nodePointer += j;   // 这样的话，我们的nodePointer就指向我们要打开的文件的FileNode
                        fileNodeOffset = j * 256 + BLOCK_SIZE * root->dataPoint[i];
                        break;
                    }
                }
                if(flag == 1 || fileExist == 1) {
                    break;
                }
            }
        }
        if(flag == 0 && fileExist == 0 && root->blockPoint != 0) {
            // 上面的循环 循环10次是遍历一级索引，下面我们遍历二级索引
            fseek(fp, BLOCK_SIZE * root->blockPoint, SEEK_SET);
            fread(blockNode, BLOCK_SIZE, 1, fp);
            for(int i = 0; i < 512; i++) {
                if(((long *)blockNode)[i] != 0) {
                    fseek(fp, BLOCK_SIZE * ((long *)blockNode)[i], SEEK_SET);
                    fread(block, BLOCK_SIZE, 1, fp);
                    for(int j = 0; j < 16; j++) {
                        if(nodePointer[j].valid == 1 && nodePointer[j].type == 1 && strcmp(nodePointer[j].filename, path) == 0) {
                            // 找到该FileNode
                            flag = 1;
                            targetNode = nodePointer + j;
                            targetOffset = j * 256 + BLOCK_SIZE * ((long *)blockNode)[i];
                            break;
                        }
                        if(nodePointer[j].valid == 1 && nodePointer[j].type == 0 && strcmp(nodePointer[j].filename, path) == 0) {
                            // 找到了一个和path同名的文件，而不是目录
                            // 有两种情况，第一就是输入的路径有问题
                            // 第二就是当前的path已经是最终的文件名了，我们要打开的文件就是这个nodePointer[j]
                            fileExist = 1;
                            nodePointer += j;
                            fileNodeOffset = j * 256 + BLOCK_SIZE * ((long *)blockNode)[i];
                            break;
                        }
                    }
                    if(flag == 1 || fileExist == 1) {
                        break;
                    }
                }
            }
        }

        if(flag == 0) {
            // 这里肯定不是文件
            free(dir);
            free(block);
            free(blockNode);
            return -6;
        } else {
            // 表示我们找到目录path了
            memcpy(root, targetNode, sizeof(struct FileNode));      // 更新root
            path = strtok(NULL, "/");
        }
        flag = 0;
    }

    // 这里，我们找到了要删除的目录，root就是我们要删除的目录的FileNode
    // 首先我们判断root是否为空
    for(int i = 0; i < 10; i++) {
        if(root->dataPoint[i] != 0) {
            fseek(fp, root->dataPoint[i] * BLOCK_SIZE, SEEK_SET);
            fread(block, BLOCK_SIZE, 1, fp);
            for(int j = 0; j < 256; j++) {
                if(nodePointer[j].valid == 1) {
                    free(dir);
                    free(block);
                    free(blockNode);
                    return -7;
                    // 不是空目录 不允许删除
                }
            }
            releaseBlock(root->dataPoint[i], fp);
            root->dataPoint[i] = 0;
        }
    }

    memset(root, 0, sizeof (struct FileNode));
    fseek(fp, targetOffset, SEEK_SET);
    fwrite(root, sizeof(struct FileNode), 1, fp);

    free(dir);
    free(block);
    free(blockNode);
    return 0;
}


unsigned long getBlock(FILE *fp) {
    long offset = ftell(fp);
    long tempOff;
    fseek(fp, BLOCK_SIZE, SEEK_SET);
    char bitmap[BLOCK_SIZE];
    for(int i = 0; i < 16; i++) {
        fread(bitmap, BLOCK_SIZE, 1, fp);
        for(int j = 0; j < BLOCK_SIZE; j++) {
            if((bitmap[j] ^ 0xff) == 0) {
                continue;
            }
            // 该8位存在0
            for(int k = 0; k < 8; k++) {
                if(((bitmap[j] >> k) & 1) == 0) {
                    // 第k位为0，可以分配
                    bitmap[j] = bitmap[j] | (1 << k);
                    tempOff = ftell(fp);
                    fseek(fp, tempOff - BLOCK_SIZE, SEEK_SET);
                    fwrite(bitmap, BLOCK_SIZE, 1, fp);
                    fseek(fp, offset, SEEK_SET);
                    return i * BLOCK_SIZE * 8 + j * 8 + k + 17;
                }
            }
        }
    }
    return 0;
}

int releaseBlock(unsigned long block, FILE *fp) {
    long offset = ftell(fp);
    int bitmapBlock = (block - 17) / (4096 * 8) + 1;
    fseek(fp, BLOCK_SIZE * bitmapBlock, SEEK_SET);
    char bitmap[BLOCK_SIZE];
    fread(bitmap, BLOCK_SIZE, 1, fp);
    int bitmapLoc = (block - 17) % (4096 * 8);
    bitmap[bitmapLoc / 8] = ~(~(bitmap[bitmapLoc / 8]) | (1 << (bitmapLoc % 8)));
    fseek(fp, BLOCK_SIZE * bitmapBlock, SEEK_SET);
    fwrite(bitmap, BLOCK_SIZE, 1, fp);
    fseek(fp, offset, SEEK_SET);
    return 0;
}









