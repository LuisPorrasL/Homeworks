#include "Sem.h"

Sem::Sem(int initVal){
    id = semget(S_KEY, 1,IPC_CREAT | 0600);
    if(-1 == id){
        perror("Sem::Sem");
        _exit(-1);
    }
    cout << "Sem ID: " << id << endl;
    semun u;
    u.val = initVal;
    if(-1 == semctl(id, 0, SETVAL, u)){
        perror("Sem::Sem");
        _exit(-1);
    }
}

Sem::~Sem(){
    if(-1 == semctl(id, 0, IPC_RMID)){
        perror("Sem::~Sem");
        _exit(-1);
    }
    cout << "Sem: ¡Me muero!" << '\n';
}
int Sem::Signal(){
    sembuf s;
    s.sem_num = 0;
    s.sem_op = 1;
    s.sem_flg = 0;

    return semop(id, &s, 1);
}
int Sem::Wait(){
    sembuf s;
    s.sem_num = 0;
    s.sem_op = -1;
    s.sem_flg = 0;

    return semop(id, &s, 1);
}
