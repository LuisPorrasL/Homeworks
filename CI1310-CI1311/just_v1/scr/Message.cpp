#include "Message.h"

Message::Message()
{
    id = msgget(M_KEY, IPC_CREAT | 0600);
    if(-1 == id){
        perror("Message::Message");
        _exit(-1);
    }
}

Message::~Message()
{
    if(-1 == msgctl(id, IPC_RMID, NULL)){
        perror("Message::~Message");
        _exit(-1);
    }
    cout << "Message: ¡Me muero!" << '\n';
}

int Message::send(const char* m)
{
    msgbuf a;
    a.mtype = 1;
    strcpy(a.mtext, m);
    int x = msgsnd(id, (void*)&a, sizeof(a), IPC_NOWAIT);
    if(-1 == x){
        perror("Message::send");
        _exit(-1);
    }
    return x;
}

int Message::receive(char* m, int len)
{
    msgbuf a;
    int x = msgrcv(id, (void*)&a, sizeof(a), 1, 0);
    if(-1 == x){
        perror("Message::receive");
        _exit(-1);
    }
    else{
        strncpy(m, a.mtext, len);
    }
    return x;
}
