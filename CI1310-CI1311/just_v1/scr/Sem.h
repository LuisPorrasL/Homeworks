#ifndef SEM_H
#define SEM_H
#define S_KEY 0xB65477

#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <unistd.h>

using namespace std;

class Sem
{
private:
    union semun{
        int              val;    /* Value for SETVAL */
        struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
        unsigned short  *array;  /* Array for GETALL, SETALL */
        struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                               (Linux-specific) */
    };
public:
    Sem(int initVal = 0);
    ~Sem();
    int Signal();
    int Wait();
private:
    int id;
};

#endif // SEM_H
