#ifndef MESSAGE_H
#define MESSAGE_H
#define M_KEY 0xC65477 // Valor de la llave del recurso
#define M 32

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

using namespace std;

class Message
{
private:
    struct msgbuf {
        long mtype;       //message type, must be > 0
        int counter;
        char mtext[M];    //message data
    };
public:
    Message();
    ~Message();
    int send(long msgtype, const char* m, int c);
    int receive(char *m, int len, int *c, long msgtype);// len es el tamaño máximo que soporte la variable mensaje
private:
    int id;// Identificador del buzon
};

#endif // MESSAGE_H
