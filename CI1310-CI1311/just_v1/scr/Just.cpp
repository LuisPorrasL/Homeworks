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
        int sonID = 1;
        for(list<string>::iterator ind = argS.inputFileNames.begin(); ind != argS.inputFileNames.end(); ++ind){
            if(!fork()){
                indent(*ind, getOutputFileName(*ind), sonID);
                _exit(0);
            }
            ++sonID;
        }
        int sonN = sonID;
        cout << sonN << '\n';
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

void Just::indent(const string& iFName, const string& oFName, int sonID)
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
        sendReservedWordData(sonID);
        file.write(justList, oFName);
    }
    else{
        sendReservedWordData(sonID);
    }
}

list<string> Just::splitInstructions(string& line)
{
    list<string>instructionList;
    size_t actualP = 0;
    size_t instP = findFirstValidOf(line,INSTRUCTION_DELIMETERS, actualP);
    string instruction;

    while(instP != string::npos){

        instruction = line.substr(actualP, instP-actualP+1);

        //Special cases
        //for loop
        if(findValid(instruction, FOR) != string::npos){
            if(instruction.back() == ';'){
                size_t pPos = findFirstValidOf(line, SEMICOLON, instP + 1);
                if(pPos != string::npos){
                    actualP = instP + 1;
                    instP = pPos;
                    instruction.append(line.substr(actualP, instP-actualP+1));
                    pPos = findFirstValidOf(line, CLOSE_P, instP + 1);
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
            size_t sCP = line.find_first_not_of(SPACES, instP+1);
            if(sCP != string::npos && line[sCP] == ';'){
                actualP = instP + 1;
                instP = sCP;
                instruction.append(line.substr(actualP, instP-actualP+1));
            }
        }

        actualP = instP+1;
        instP = findFirstValidOf(line, INSTRUCTION_DELIMETERS, actualP);

        if(instP == string::npos && actualP < line.size()){
            string instComment = line.substr(actualP);
            if(instComment.find_first_not_of(SPACES) != string::npos){
                if(instComment.find(COMMENT) != string::npos){             //If there if a comment at the end of the instruction
                    instruction.append(instComment);
                }
                else{                                                   //If there is another instruction
                    line = instComment;
                    actualP = 0;
                }
            }
        }

        //Separete {} parenthesis from each instruction if needed
        size_t pPos = findFirstValidOf(instruction, BRACES);
        if(pPos != string::npos){
            string subInstruction = instruction.substr(pPos);
            instruction.erase(pPos);
            if(instruction.find_first_not_of(SPACES) != string::npos){
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

    if(findFirstValidOf(instruction, CLOSE_BRACE) != string::npos){
        if(pCounter > 0){
            --pCounter;
        }
    }
    addSpaces(instruction, argS.e*pCounter);
    justList->push_back(instruction);

    if(findFirstValidOf(instruction, OPEN_BRACE) != string::npos){
        ++pCounter;
    }
}

void Just::deleteSpaces(string& str)
{
    size_t found = str.find_first_not_of(SPACES);
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
        replaceBetweenChar(cInstruction, CITATION_MARK);
        replaceBetweenChar(cInstruction, APOSTOPHE);

        //Now I need to separete str into token
        queue<string> iTokens = tokenize(cInstruction, CODE_DELIMETERS);

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
    replaceBetweenChar(cLine, CITATION_MARK);
    replaceBetweenChar(cLine, APOSTOPHE);

    size_t strPos = cLine.find_first_of(str, pos);
    return strPos;
}

size_t Just::findValid(const string& line, const string& str, size_t pos)
{
    string cLine = line;
    replaceComments(cLine);
    replaceBetweenChar(cLine, CITATION_MARK);
    replaceBetweenChar(cLine, APOSTOPHE);

    size_t strPos = cLine.find(str, pos);
    return strPos;
}

void Just::replaceComments(string& str)
{
    size_t pPos = str.find(COMMENT);
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

void Just::sendReservedWordData(int mtype){

}
