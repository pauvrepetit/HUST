#ifndef FUNC_H
#define FUNC_H
#include <QString>

unsigned int IPtoINT(QString IPAddr);
QString INTtoIP(unsigned int IPAddr);
void closeSocket(int socket);
#endif // FUNC_H
