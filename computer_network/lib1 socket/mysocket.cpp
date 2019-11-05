#include "mysocket.h"
#include "func.h"
#include <iostream>
#include <signal.h>
#include <string>
#include <curl/curl.h>

using std::string;

Widget *thisWidget;
int serverSocket;
string baseDir;

#define BUFFER_LENGTH (1 << 20)
#define METHOD_LENGTH (1 << 10)
#define FILENAME_LENGTH (1 << 12)

void brokenPipe(int signum) {
    error(signum, thisWidget); // brokenPipe signal is 13
    success(10, thisWidget);   // 输出线程终止信息
    pthread_exit(NULL);
    return;
}

void *serverMain(void *argv) {
    // 设置cancel状态
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,
                           NULL); // 可以被其他线程调用pthread_cancel终止
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,
                          NULL); // 接受到终止请求后立即终止

    signal(SIGPIPE, brokenPipe);

    serverMainArg *arg = (serverMainArg *)argv;
    int *returnCode = new int;
    thisWidget = arg->widget;
    baseDir = arg->baseDir.toUtf8().constData();
    *returnCode = makeConnection(arg->port, arg->address, arg->listenQueueLen,
                                 thisWidget, &serverSocket); // 建立连接
    if (*returnCode != 0) {
        // makeConnection 失败
        return (void *)returnCode;
    }

    int clientSocket;
    struct sockaddr_in clientSockAddr;
    unsigned int clientSockLen;
    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientSockAddr,
                              &clientSockLen);
        // 等待到serverSocket套接字的连接，给cllientSockAddr和clientSockLen赋值，返回生成的响应套接字
        if (clientSocket == -1) {
            // accept() error
            close(serverSocket);
            close(clientSocket);
            error(4, thisWidget);
            *returnCode = 4;
            return (void *)returnCode;
        }
        // now get a connection
        // create a thread to process it
        pthread_t newThread;
        struct clientArg arg;
        arg.clientSocket = clientSocket;
        arg.clientSockAddr = clientSockAddr;
        pthread_create(&newThread, NULL, processClient, (void *)&arg);
    }
    return (void *)returnCode;
}

int error(int errorCode, Widget *widget) {
    QString threadNum;
    switch (errorCode) {
    case 1:
        // socket 创建失败
        emit widget->sendMsg("socket 创建失败");
        break;
    case 2:
        // socket 绑定失败
        emit widget->sendMsg("socket 绑定(bind)失败");
        break;
    case 3:
        // socket 监听失败
        emit widget->sendMsg("socket 监听失败");
        break;
    case 4:
        // socket 连接失败
        threadNum.setNum(pthread_self());
        emit widget->sendMsg("ThreadID:" + threadNum +
                             " socket 连接(accept)失败");
        break;
    case 5:
        // socket报文中不包含HTTP字段，无法识别该报文
        threadNum.setNum(pthread_self());
        emit widget->sendMsg("ThreadID:" + threadNum +
                             " socket报文中不包含HTTP字段，无法识别该报文");
        break;
    case 6:
        // HTTP报文不是GET请求
        threadNum.setNum(pthread_self());
        emit widget->sendMsg("ThreadID:" + threadNum + " HTTP报文不是GET请求");
        break;
        //    case 7:
        //        // 未知的文件类型
        //        threadNum.setNum(pthread_self());
        //        emit widget->sendMsg("ThreadID:" + threadNum + "
        //        未知的文件类型"); break;
    case 8:
        // 404
        threadNum.setNum(pthread_self());
        emit widget->sendMsg("ThreadID:" + threadNum + " 文件不存在");
        break;
    case 13:
        // broken pipe
        threadNum.setNum(pthread_self());
        emit widget->sendMsg("ThreadID:" + threadNum +
                             " 连接被客户端断开(BrokenPipe)");
        break;
    default:
        break;
    }
    return errorCode;
}

int success(int successCode, Widget *widget) {
    QString threadNum;
    switch (successCode) {
    case 1:
        // socket 创建成功
        emit widget->sendMsg("socket 创建成功");
        break;
    case 2:
        // socket 绑定成功
        emit widget->sendMsg("socket 绑定(bind)成功");
        break;
    case 3:
        // socket 监听成功
        emit widget->sendMsg("socket 监听成功");
        break;
    case 4:
        // socket 连接成功
        threadNum.setNum(pthread_self());
        emit widget->sendMsg("ThreadID:" + threadNum +
                             " socket 连接(accept)成功");
        break;
    case 9:
        // html文件发送成功
        threadNum.setNum(pthread_self());
        emit widget->sendMsg("ThreadID:" + threadNum + " 文件发送成功");
        break;
    case 10:
        // 线程结束
        threadNum.setNum(pthread_self());
        emit widget->sendMsg("ThreadID:" + threadNum + " 线程结束");
        break;
    default:
        break;
    }
    return successCode;
}

int makeConnection(unsigned short port, unsigned int address,
                   int listenQueueLen, Widget *widget, int *serverSocket) {
    int errorResult;

    *serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    // socket函数 创建套接字
    // AF_INET 指定地址类型为ipv4
    // SOCK_STREAM 流式套接字
    // 0 表示自定选择传输协议(TCP/UDP)
    if (*serverSocket == -1) {
        // socket() error
        return error(1, widget);
    } else {
        success(1, widget);
    }

    // struct sockaddr serverSockAddr;     // socket address (套接字地址)
    struct sockaddr_in
        serverSockAddr; // 将struct sockaddr中14字节的sa_data细化为几个小部分
    memset(&serverSockAddr, 0, sizeof(serverSockAddr));
    serverSockAddr.sin_family = AF_INET;   // address family
    serverSockAddr.sin_port = htons(port); // port
    // serverSockAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Internet address
    serverSockAddr.sin_addr.s_addr = htonl(address);
    // htons() and htonl() 转换字节序

    errorResult = bind(*serverSocket, (struct sockaddr *)&serverSockAddr,
                       sizeof(serverSockAddr));
    // give the socket the address 将套接字和地址绑定
    if (errorResult == -1) {
        // bind() error
        close(*serverSocket);
        return error(2, widget);
    } else {
        success(2, widget);
    }

    errorResult =
        listen(*serverSocket,
               listenQueueLen); // 监听套接字 listenQueueLen为最大排队数量
    if (errorResult == -1) {
        // listen() error
        close(*serverSocket);
        return error(3, widget);
    } else {
        success(3, widget);
    }

    return 0;
}

void outputHTTPRequest(char *readBuffer, Widget *widget) {
    QString threadNum;
    threadNum.setNum(pthread_self());
    QString HTTPRequest(readBuffer);
    emit widget->sendMsg("ThreadID:" + threadNum + "\nHTTP请求：" + HTTPRequest);
    return;
}

void *processClient(void *arg) {
    success(4, thisWidget); // 输出log 创建连接成功
    outputConnInfo(((struct clientArg *)arg)->clientSockAddr, thisWidget);
    int clientSocket = ((struct clientArg *)arg)->clientSocket;
    char *readBuffer = (char *)malloc(sizeof(char) * BUFFER_LENGTH);
    char *readBufferBegin = readBuffer; // use this pointer to free memory

    read(clientSocket, readBuffer, BUFFER_LENGTH - 1);
    // 从套接字clientSocket中读取最多BUFFER_LENGTH字节的数据到readBuffer中
    // 此处读取的应该为HTTP请求报文

    outputHTTPRequest(readBuffer, thisWidget); // 输出HTTP请求报文


    if (!strstr(readBuffer, "HTTP/")) {
        // socket报文中不存在HTTP/子串，不是HTTP请求报文，返回错误
        error(5, thisWidget);
        sendErrorData(5, clientSocket);
        free(readBufferBegin);
        success(10, thisWidget);
        return NULL;
    }

    char HTTPMethod[METHOD_LENGTH];
    char HTTPGetFilename[FILENAME_LENGTH];

    strcpy(HTTPMethod, strsep(&readBuffer, " /"));
    strcpy(HTTPGetFilename, strsep(&readBuffer, " ") + 1);

    if (strcmp(HTTPMethod, "GET") != 0) {
        // HTTP method is not GET 返回错误
        error(6, thisWidget);
        sendErrorData(6, clientSocket);
        free(readBufferBegin);
        success(10, thisWidget);
        return NULL;
    }

    CURL *c = curl_easy_init();
    char *filenameTrans = curl_easy_unescape(c, HTTPGetFilename, 0, NULL);
    // 将文件名转化为可识别的模式
    curl_easy_cleanup(c);

    int sendStatus = sendData(clientSocket, filenameTrans); // 发送数据
    // return 8 => 文件不存在
    // return 0 => 成功发送
    if (sendStatus != 0) {
        error(sendStatus, thisWidget);
    } else {
        success(9, thisWidget);
    }

    curl_free(filenameTrans);
    free(readBufferBegin);
    success(10, thisWidget);
    return NULL;
}

int outputConnInfo(struct sockaddr_in clientSockAddr, Widget *widget) {
    QString threadNum;
    QString port;
    QString IP = INTtoIP(clientSockAddr.sin_addr.s_addr);
    threadNum.setNum(pthread_self());
    port.setNum(ntohs(clientSockAddr.sin_port));
    emit widget->sendMsg("ThreadID:" + threadNum + " 连接信息：IP:" + IP +
                         " Port:" + port);
    return 0;
}

int sendData(int socket, char *filename) {
    char *bufferFilename = (char *)malloc(sizeof(char) * FILENAME_LENGTH);
    char *bufferFilenameBegin = bufferFilename; // for free
    strcpy(bufferFilename, filename);

    strsep(&bufferFilename, ".");
    if (bufferFilename == NULL) {
        // 文件名中没有. 没有扩展名 当作未知文件类型处理
        free(bufferFilenameBegin);
        return sendKnownFile(socket, filename, 3);
    }
    // 此处将bufferFilename指针指向字符串中第一个.的后面一个位置，即扩展名的位置

    if (strcmp(bufferFilename, "html") == 0) {
        // get html file
        free(bufferFilenameBegin); //  事实上到这里bufferFilename数组就用不上了
        return sendKnownFile(socket, filename, 0);
    } else if (strcmp(bufferFilename, "png") == 0) {
        // get png image
        free(bufferFilenameBegin);
        return sendKnownFile(socket, filename, 1);
    } else if (strcmp(bufferFilename, "mp4") == 0) {
        // get mp4 video
        free(bufferFilenameBegin);
        return sendKnownFile(socket, filename, 2);
    } else {
        // 未知的文件类型 发送文件
        free(bufferFilenameBegin);
        return sendKnownFile(socket, filename, 3);
    }
}

// 发送文件
// return 8 => file not exist
// return 0 => file send success
int sendKnownFile(int socket, char *filename, int type) {
    string status = "HTTP/1.1 200 OK\r\n";
    string header;
    switch (type) {
    case 0:
        header = "Content-Type: text/html\r\n\r\n";
        break;
    case 1:
        header = "Content-Type: image/png\r\n\r\n";
        break;
    case 2:
        header = "Context-Type: video/mpeg4\r\n\r\n";
        break;
    case 3:
        header = "Context-Type: application/octet-stream\r\n\r\n";
        break;
    }

    string filenameStr(filename);
    string filePath = baseDir + filenameStr;

    FILE *fp = fopen(filePath.c_str(), "r");
    char buffer[BUFFER_LENGTH];
    if (fp == NULL) {
        // 404 Not Found
        send404(socket);
        return 8;
    } else {
        // 文件存在 读取文件 发送
        write(socket, status.c_str(), status.length());
        write(socket, header.c_str(), header.length());
        int readLen = fread(buffer, 1, sizeof(buffer), fp);
        while (!feof(fp)) {
            // 还没有读完该文件
            write(socket, buffer, sizeof(buffer));
            readLen = fread(buffer, 1, sizeof(buffer), fp);
        }
        write(socket, buffer, readLen);
        // 至此，将整个文件读取完成，并写入到socket中
        fclose(fp);
        close(socket);
        return 0;
    }
}

int send404(int socket) {
    string status = "HTTP/1.1 404 Not Found\r\n";
    string header = "Content-Type: text/html\r\n\r\n";
    string body = "<html><head><title>404 Not Found</title></head><body><p>404 "
                  "Not Found\r\n</p></body></html>";

    write(socket, status.c_str(), status.length());
    write(socket, header.c_str(), header.length());
    write(socket, body.c_str(), body.length());

    close(socket);
    return 0;
}

int sendErrorData(int errorCode, int clientSocket) {
    string errorMessage;
    switch (errorCode) {
    case 5:
        errorMessage = "socket报文中不包含HTTP字段，无法识别该报文";
        break;
    case 6:
        errorMessage = "HTTP报文不是GET请求";
        break;
    default:
        break;
    }

    string status = "HTTP/1.1 400 Bad Request\r\n";
    string header = "Content-Type: text/html\r\n\r\n";
    string body = "<html><head><title>Bad Request</title></head><body><p>400 "
                  "Bad Request\r\n</p><p>" +
                  errorMessage + "</p></body></html>";

    write(clientSocket, status.c_str(), status.length());
    write(clientSocket, header.c_str(), header.length());
    write(clientSocket, body.c_str(), body.length());
    close(clientSocket);
    return 0;
}
