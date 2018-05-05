#include "Just.h"

Just::Just(): pCounter(0), file(), argS(), rWStructure()
{

}

Just::~Just()
{

}

void Just::justify(int argc, char* argv[])
{
    initializeArguments(argc, argv);
    if(argS.isHelpAsked){
        printHelp();
        return;
    }
    if(!(argS.errorCode == -1) && !argS.inputFileNames.empty()){
        //I need to load the reserved words form the ReservedWords.txt file and store then on rWStructure.
        list<string> rWList = file.read("ReservedWords.txt");
        for(list<string>::iterator ind = rWList.begin(); ind != rWList.end(); ++ind){
            rWStructure[*ind] = 0;
        }

        //I need to create a subprocess (son) for each inputfile and tell each son to indent it's inputfile.
        //I need to create a "Buzon" to comunicate with my sons.
        Message m;
        int sonID = 1;
        int sonN = argS.inputFileNames.size();
        for(list<string>::iterator ind = argS.inputFileNames.begin(); ind != argS.inputFileNames.end(); ++ind){
            if(!fork()){
                indent(*ind, getOutputFileName(*ind), m, sonID);
                _exit(0);
            }
            ++sonID;
        }

        //I need to store the number of sons I have created.
        //I need to recive the messages from my sons and store them in shared memory.
        Sem s;

        int shmID = shmget(SHM_KEY, sizeof(shmStruct), IPC_CREAT | 0600);
        if(-1 == shmID){
            perror("Just::justify: shmget error");
            exit(-1);
        }
        shmStruct* shmArea = (shmStruct*)shmat(shmID, NULL, 0);

        //I need to create a son to read from shmArea and print on the stdout
        int rWFirstLetterCounter = countReservedWordsFistLetters();
        if(!fork()){
            for(int ind = 0; ind < rWFirstLetterCounter; ++ind){
                s.wait();
                cout << '\n';
                for(int shmInd = 0; shmInd < shmArea->rMNumber; ++shmInd){
                    char rWData[M] = {0};
                    sprintf(rWData, "%-16s :%4d", shmArea->rMArray[shmInd].word, shmArea->rMArray[shmInd].counter);
                    cout << rWData << '\n';
                }
                s.signal();
            }
            _exit(0);
        }

        int rWN = rWStructure.size();
        int rMCounter = 0;
        char rWFirstLetter = 'a';
        for(int rWInd = 0; rWInd < rWN; ++rWInd){
            recivedMessage rM, shmM;
            for(sonID = 1; sonID <= sonN; ++sonID){
                m.receive(rM.word, M, &rM.counter, sonID);
                if(sonID == 1){
                    strncpy(shmM.word, rM.word, M);
                }
                shmM.counter += rM.counter;
            }
            if(shmM.word[0] == rWFirstLetter){
                shmArea->rMArray[rMCounter] = shmM;
            }
            else{
                shmArea->rMNumber = rMCounter;
                s.signal();
                s.wait();
                rMCounter = 0;
                shmArea->rMArray[rMCounter] = shmM;
                while(rWFirstLetter != shmM.word[0]){
                    rWFirstLetter++;
                }
            }
            rMCounter++;
        }
        shmArea->rMNumber = rMCounter;
        s.signal();
        s.wait();

        shmdt(shmArea);
        shmctl(shmID, IPC_RMID, NULL);
    }
    else{
        printError();
    }
}

string Just::getOutputFileName(const string& iFName){
    string inFileName = iFName;
    size_t found = inFileName.find_last_of("/\\");
    if(found != string::npos){
        inFileName = inFileName.substr(found + 1);
    }
    queue<string> inFileNameTokens = Just::tokenize(inFileName, ".");
    string outFileName = inFileNameTokens.front();
    outFileName.append(".sgr");

    return outFileName;
}

int Just::countReservedWordsFistLetters(){
    int firstLettersCounter = 0;
    char firstLetter = ' ';
    for(map<string, int>::iterator it = rWStructure.begin(); it != rWStructure.end(); ++it){
        if(it->first[0] != firstLetter){
            firstLetter = it->first[0];
            firstLettersCounter++;
        }
    }
    return firstLettersCounter;
}

queue<string> Just::tokenize(const string &str, const char* delimeters)
{
    queue<string> strTokens;

    char* cStr = new char [str.length()+1];
    strcpy(cStr, str.c_str());
    char* p = strtok(cStr, delimeters);
    while(p != 0){
        strTokens.push(p);
        p = strtok(NULL, delimeters);
    }
    delete[] cStr;

    return strTokens;
}

void Just::indent(const string& iFName, const string& oFName, Message& m, int sonID)
{
    list<string> fileList = file.read(iFName);
    if(!fileList.empty()){
        list<string> justList;
        list<string> instructionList;
        //For each line of code
        for(list<string>::iterator indF = fileList.begin(); indF != fileList.end(); ++indF){
            instructionList = splitInstructions(*indF);
            //For each instruction
            for(list<string>::iterator indI = instructionList.begin(); indI != instructionList.end(); ++indI){
                countReservedWords(*indI);
                indentInstruction(&justList, *indI);
            }
        }
        sendReservedWordData(m, sonID);
        file.write(justList, oFName);
    }
    else{
        sendReservedWordData(m, sonID);
    }
}

list<string> Just::splitInstructions(string& line)
{
    list<string>instructionList;
    size_t actualP = 0;
    size_t instP = findFirstValidOf(line,instructionDelimeters, actualP);
    string instruction;

    while(instP != string::npos){

        instruction = line.substr(actualP, instP-actualP+1);

        //Special cases
        //for loop
        if(findValid(instruction, forS) != string::npos){
            if(instruction.back() == ';'){
                size_t pPos = findFirstValidOf(line, semicolon, instP + 1);
                if(pPos != string::npos){
                    actualP = instP + 1;
                    instP = pPos;
                    instruction.append(line.substr(actualP, instP-actualP+1));
                    pPos = findFirstValidOf(line, closeP, instP + 1);
                    if(pPos != string::npos){
                        actualP = instP + 1;
                        instP = pPos;
                        instruction.append(line.substr(actualP, instP-actualP+1));
                    }
                }
            }
        }
        //};
        if(instruction.back() == '}'){
            size_t sCP = line.find_first_not_of(spaces, instP+1);
            if(sCP != string::npos && line[sCP] == ';'){
                actualP = instP + 1;
                instP = sCP;
                instruction.append(line.substr(actualP, instP-actualP+1));
            }
        }

        actualP = instP+1;
        instP = findFirstValidOf(line, instructionDelimeters, actualP);

        if(instP == string::npos && actualP < line.size()){
            string instComment = line.substr(actualP);
            if(instComment.find_first_not_of(spaces) != string::npos){
                if(instComment.find(comment) != string::npos){             //If there if a comment at the end of the instruction
                    instruction.append(instComment);
                }
                else{                                                   //If there is another instruction
                    line = instComment;
                    actualP = 0;
                }
            }
        }

        //Separete {} parenthesis from each instruction if needed
        size_t pPos = findFirstValidOf(instruction, braces);
        if(pPos != string::npos){
            string subInstruction = instruction.substr(pPos);
            instruction.erase(pPos);
            if(instruction.find_first_not_of(spaces) != string::npos){
                instructionList.push_back(instruction);
            }
            instructionList.push_back(subInstruction);
        }
        else{
            instructionList.push_back(instruction);
        }
    }
    //If the line is only a comment.
    if(actualP == 0){
        instruction = line;
        instructionList.push_back(instruction);
    }
    return instructionList;
}

void Just::indentInstruction(list<string>* justList, string instruction)
{
    deleteSpaces(instruction);

    if(findFirstValidOf(instruction, closeBrace) != string::npos){
        if(pCounter > 0){
            --pCounter;
        }
    }
    addSpaces(instruction, argS.e*pCounter);
    justList->push_back(instruction);

    if(findFirstValidOf(instruction, openBrace) != string::npos){
        ++pCounter;
    }
}

void Just::deleteSpaces(string& str)
{
    size_t found = str.find_first_not_of(spaces);
    if(found!=string::npos){
        str = str.substr(found);
    }
    else{
        str.clear();
    }
}

void Just::addSpaces(string& str, int n)
{
    str.insert(str.begin(), n, SPACE);
}

void Just::countReservedWords(const string& instruction)
{
    if(!instruction.empty()){
        string cInstruction = instruction;
        //First I need to make sure I don't count reserved word in comments or between " or '.
        replaceComments(cInstruction);
        replaceBetweenChar(cInstruction, citationMark);
        replaceBetweenChar(cInstruction, apostrophe);

        //Now I need to separete str into token
        queue<string> iTokens = tokenize(cInstruction, codeDelimeters.c_str());

        //Finaly I need to check if a tokens is a reserved word, and if it is, update it's counter.
        map<string, int>::iterator found;
        while(!iTokens.empty()){
            found = rWStructure.find(iTokens.front());
            if(found != rWStructure.end()){
                ++found->second;
            }
            iTokens.pop();
        }
    }
}

size_t Just::findFirstValidOf(const string& line, const string& str, size_t pos)
{
    string cLine = line;
    replaceComments(cLine);
    replaceBetweenChar(cLine, citationMark);
    replaceBetweenChar(cLine, apostrophe);

    size_t strPos = cLine.find_first_of(str, pos);
    return strPos;
}

size_t Just::findValid(const string& line, const string& str, size_t pos)
{
    string cLine = line;
    replaceComments(cLine);
    replaceBetweenChar(cLine, citationMark);
    replaceBetweenChar(cLine, apostrophe);

    size_t strPos = cLine.find(str, pos);
    return strPos;
}

void Just::replaceComments(string& str)
{
    size_t pPos = str.find(comment);
    if(pPos != string::npos){
        str.replace(pPos, str.size()-pPos, str.size()-pPos, NULL_C);
    }
}

void Just::replaceBetweenChar(string& str, const string& c)
{
    size_t firstP = str.find_first_of(c);
    size_t secondP = 0;
    while(firstP != string::npos){
        secondP = str.find_first_of(c, firstP + 1);
        if(secondP != string::npos){
            str.replace(firstP, secondP + 1 - firstP, secondP + 1 - firstP, NULL_C);
            firstP = str.find_first_of(c, secondP + 1);
        }
        else{
            firstP = string::npos;
        }
    }
}

void Just::initializeArguments(int argc, char* argv[])
{
    for(int ind = 1; ind < argc; ++ind){

        if(0 == strcmp(argv[ind], "--help")){
            argS.isHelpAsked = true;
            return;
        }

        if(0 == strcmp(argv[ind], "-i")){
            if(ind + 1 < argc){
                argS.inputFileNames.push_back(argv[++ind]);
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

        else{
            argS.inputFileNames.push_back(argv[ind]);
        }
    }
}

void Just::printHelp()
{
    cout << help << '\n';
}

void Just::printError()
{
    cerr << error << '\n';
}

void Just::sendReservedWordData(Message& m, int mtype){
    for(map<string, int>::iterator it = rWStructure.begin(); it != rWStructure.end(); ++it){
        m.send(mtype, it->first.c_str(), it->second);
    }
}
