#ifndef JUSTCONTROLLER_H
#define JUSTCONTROLLER_H
#define DEFAULT_E 4
#define N 1024

#include "File.h"
#include "Just.h"

#include <iostream>

using namespace std;

class JustController
{
private:
    struct argumentsS{
        int errorCode;
        int e;
        bool isHelpAsked;
        string inFileName;
        string outFileName;
        bool all;

        inline argumentsS():errorCode(0), e(0), isHelpAsked(false),
            inFileName{}, outFileName{}, all(false){}
    };
    static const string help;
    static const string error;

private:

    File file;
    Just just;
    argumentsS argS;

public:

    JustController();
    ~JustController();
    //"Initialize" argS based on the arguments past by the user.
    void initArguments(int argc, char* argv[]);
    void run();

private:

    void printHelp();
    void printError();

};

#endif // JUSTCONTROLLER_H
