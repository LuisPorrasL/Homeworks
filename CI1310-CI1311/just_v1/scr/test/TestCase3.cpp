#include "Just.h"

//Static const variables initialization
const string Just::spaces = " \t";
const string Just::encabesadoRWD = "Number of reserved words found: ";
const char* Just::rWDelimeters = " (){}:<>*&;";

Just::Just(): e(0), pCounter(0){
    rWCounter = new int[M];
}

Just::~Just(){
    delete[] rWCounter;
}

list<string>Just::justify(list<string> fList, int userE){

    e = userE;
    list<string>justFile;
    string line;

    //For each line of code
    for(list<string>::iterator ind = fList.begin(); ind != fList.end(); ++ind){
        line = *ind;

        //First I need to check if there are multiple instructions per line. If there are I need to separete them into sigle instructions.
        size_t actualP = 0;
        size_t instP = findValidFirstOf(line,"{};", actualP);
        string instruction;

        while(instP != string::npos){

            instruction = line.substr(actualP, instP-actualP+1);

            //Special cases
            //for loop
            if(instruction.find("for ") != string::npos || instruction.find("for(") != string::npos){
                if(instruction.back() == ';'){
                    size_t pPos = findValidFirstOf(line, ")", instP + 1);
                    if(pPos != string::npos){
                        actualP = instP + 1;
                        instP = pPos;
                        instruction.append(line.substr(actualP, instP-actualP+1));
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
            instP = findValidFirstOf(line,"{};", actualP);

            if(instP == string::npos && actualP < line.size()){
                string instComment = line.substr(actualP);
                if(instComment.find_first_not_of(spaces) != string::npos){
                    if(instComment.find("//") != string::npos){             //If there if a comment at the end of the instruction
                        instruction.append(instComment);
                    }
                    else{                                                   //If there is another instruction
                        line = instComment;
                        actualP = 0;
                    }
                }
            }

            //Separete {} parenthesis from each instruction if needed
            size_t pPos = findValidFirstOf(instruction, "{}");
            if(pPos != string::npos){
                string subInstruction = instruction.substr(pPos);
                instruction.erase(pPos);
                if(instruction.find_first_not_of(spaces) != string::npos){
                    indentInst(instruction, &justFile);
                }
                indentInst(subInstruction, &justFile);
            }
            else{
                indentInst(instruction, &justFile);
            }
        }
        //If the line is only a comment.
        if(actualP == 0){
            instruction = line;
            indentInst(instruction, &justFile);
        }
    }

    return justFile;
}

queue<string> Just::tokenize(const string &line, const char* delimeters){
    queue<string> lineTokens;

    char* cLine = new char [line.length()+1];
    strcpy(cLine, line.c_str());
    char* p = strtok(cLine, delimeters);
    while(p != 0){
        lineTokens.push(p);
        p = strtok(NULL, delimeters);
    }
    delete[] cLine;

    return lineTokens;
}

size_t Just::findValidFirstOf(string line, const string& str, size_t pos){
    replaceComment(line);
    replaceCharRange(line, "\"");
    replaceCharRange(line, "'");

    size_t strPos = line.find_first_of(str, pos);
    return strPos;
}

void Just::replaceComment(string &str){
    size_t pPos = str.find("//");
    if(pPos != string::npos){
        str.replace(pPos, str.size()-pPos, str.size()-pPos, NULL_C);
    }
}

void Just::replaceCharRange(string &str, const string &charR){
    size_t firstP = str.find_first_of(charR);
    size_t secondP = 0;
    while(firstP != string::npos){
        secondP = str.find_first_of(charR, firstP + 1);
        str.replace(firstP, secondP + 1 - firstP, secondP + 1 - firstP, NULL_C);
        firstP = str.find_first_of(charR, secondP + 1);
    }
}

void Just::indentInst(string& inst, list<string>* jF){
    deleteSpaces(inst);

    if(findValidFirstOf(inst, "}") != string::npos){
        if(pCounter > 0){
            --pCounter;
        }
    }

    addSpaces(inst, e*pCounter);
    jF->push_back(inst);

    //I need to update the reserved words counters
    countReservedWords(inst);

    if(findValidFirstOf(inst, "{") != string::npos){
        ++pCounter;
    }
}

void Just::countReservedWords(string str){
    if(!str.empty()){
        //First I need to make sure I don't count reserved word in comments or between " or '.
        replaceComment(str);
        replaceCharRange(str, "\"");
        replaceCharRange(str, "'");

        //Now I need to separete str into token
        queue<string> strTokens = tokenize(str, rWDelimeters);

        //Finaly I need to check if a tokens is a reserved word, and if it is, update it's counter.
        int found = 0;
        while(!strTokens.empty()){
            found = searchReservedWord(strTokens.front());
            if(found >= 0){
                ++rWCounter[found];
            }
            strTokens.pop();
        }
    }
}

int Just::searchReservedWord(const string& str){
    if(str.size() > 1){
        for(int ind = 0; ind < M; ++ind){
            if(rWords[ind].compare(str) == 0){
                return ind;
            }
        }
    }
    return -1;
}

void Just::deleteSpaces(string &str){
    size_t found = str.find_first_not_of(spaces);
    if(found!=string::npos){
        str = str.substr(found);
    }
    else{
        str.clear();
    }
}

void Just::addSpaces(string &str, int n){
    str.insert(str.begin(), n, ' ');
}

list<string> Just::convertReservedWordsData(bool all){
    list<string> rWDataList;
    rWDataList.push_back(encabesadoRWD + '\n');
    char rWData[25];
    for(int ind = 0; ind < M; ++ind){
        if(all){
            sprintf(rWData, "%-16s :%4d", rWords[ind].c_str(), rWCounter[ind]);
            rWDataList.push_back(rWData);
        }
        else{
            if(rWCounter[ind] > 0){
                sprintf(rWData, "%-16s :%4d", rWords[ind].c_str(), rWCounter[ind]);
                rWDataList.push_back(rWData);
            }

        }
    }
    return rWDataList;
}
