#include "JustController.h"

using namespace std;

int main(int argc, char* argv[])
{
    JustController jC;
    jC.initArguments(argc, argv);
    jC.run();
    return 0;
}
