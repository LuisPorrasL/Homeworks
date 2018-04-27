#include "JustController.h"

const string JustController::help = "Usage: ./just [options] file\n"
                                    "Options:\n"
                                    "-i <input file>                Indicates the input file path.\n"
                                    "-o <output file>               Indicates the name of the output file.\n"
                                    "-all                           List all the reserved words on the justStadistics file,\n"
                                    "                               even if they did not appear at all.\n"
                                    "-e <spaces>                    Indicate the number of spaces wanted to indent.\n"
                                    "--help                         Show this message.\n"
                                    "\n"
                                    "None of these options are obligatory.\n"
                                    "If the input file is not indicate, the input file will be the stdin by default.\n"
                                    "If the output file is not indicate and neither is the input file\n"
                                    "the default output file will be the stdout.\n"
                                    "If only the output file name is not indicated, an output file will be created \n"
                                    "and named after the input file name.";
const string JustController::error = "Error: Invalid arguments";

JustController::JustController(){}

JustController::~JustController(){}

void JustController::initArguments(int argc, char* argv[]){

    for(int ind = 1; ind < argc; ++ind){
        if(0 == strcmp(argv[ind], "--help")){
            argS.isHelpAsked = true;
            return;
        }

        if(0 == strcmp(argv[ind], "-i")){
            if(ind + 1 < argc){
                if(argS.inFileName.empty()){
                    argS.inFileName = argv[++ind];
                }
            }
            else{
                argS.errorCode = -1;
                return;
            }
        }

        else if(0 == strcmp(argv[ind], "-o")){
            if(ind + 1 < argc){
                if(argS.outFileName.empty()){
                    argS.outFileName = argv[++ind];
                }
            }
            else{
                argS.errorCode = -1;
                return;
            }
        }

        else if(0 == strcmp(argv[ind], "-e")){
            if(ind + 1 < argc){
                argS.e = atoi(argv[++ind]);
            }
            else{
                argS.errorCode = -1;
                return;
            }
        }

        else if(0 == strcmp(argv[ind], "-all")){
            argS.all = true;
        }

        else{
            if(argS.inFileName.empty()){
                argS.inFileName = argv[ind];
            }
        }
    }
}

void JustController::run(){
    if(argS.isHelpAsked){
        printHelp();
        return;
    }

    if(-1 == argS.errorCode){
        printError();
        return;
    }

    if(argS.e <= 0){
        argS.e = DEFAULT_E;
    }

    if(!argS.inFileName.empty() && argS.outFileName.empty()){

        string inFileName = argS.inFileName;
        size_t found = inFileName.find_last_of("/\\");
        if(found != string::npos){
            inFileName = inFileName.substr(found + 1);
        }
        queue<string> inFileNameTokens = Just::tokenize(inFileName, ".");
        string outFileName = "just";
        outFileName.append(inFileNameTokens.front()+".");
        outFileName.append(inFileNameTokens.back());

        argS.outFileName = outFileName;
    }

    else if(argS.inFileName.empty() && !argS.outFileName.empty()){
        argS.inFileName = "std::cin";
    }

    else if(argS.inFileName.empty() && argS.outFileName.empty()){
        argS.inFileName = "std::cin";
        argS.outFileName = "std::cout";
    }

    list<string> fL = file.read(argS.inFileName);
    list<string> jF = just.justify(fL, argS.e);
    list<string> rWDL = just.convertReservedWordsData(argS.all);
    file.write(jF, argS.outFileName);
    file.write(rWDL, "justStadistics.txt");
}

void JustController::printHelp(){
    cout << help << '\n';
}

void JustController::printError(){
    cerr << error << '\n';
}
