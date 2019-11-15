#ifndef MYSOCKET_H
#define MYSOCKET_H

#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include "widget.h"

struct serverMainArg {
    unsigned short port;
    unsigned int address;
    int listenQueueLen;
    QString baseDir;
    Widget *widget;
};

struct clientArg {
    int clientSocket;
    struct sockaddr_in clientSockAddr;
};

//int serverMain(unsigned short port, unsigned int address, int listenQueueLen, QTextBrowser *log);
void *serverMain(void *argv);
int makeConnection(unsigned short port, unsigned int address, int listenQueueLen, Widget *widget, int *serverSocket);
int error(int errorCode, Widget *widget);
int success(int successCode, Widget *widget);
void *processClient(void *arg);
int sendData(int socket, char *filename);
int sendKnownFile(int socket, char *filename, int type);
int send404(int socket);
int sendErrorData(int errorCode, int clientSocket);
int outputConnInfo(struct sockaddr_in clientSockAddr, Widget *widget);
void outputHTTPRequest(char *readBuffer, Widget *widget);

//int error(int errorCode, QTextBrowser *log);
// int error(int socket, int errorCode);
// int exitError(int errorCode);
// int sendError(int socket, std::string errorMessage);
// int sendData(int socket, char *filename);
// int sendHTML(int socket, char *filename);
// int sendError_404(int socket, std::string errorMessage);

#endif // MYSOCKET_H
