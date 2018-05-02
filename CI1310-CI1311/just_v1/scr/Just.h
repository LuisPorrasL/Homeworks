#ifndef JUST_H
#define JUST_H
#define DEFAULT_E 4
#define NULL_C '*'
#define SPACE ' '
#define SPACES " \t"
#define INSTRUCTION_DELIMETERS "{};"
#define BRACES "{}"
#define OPEN_BRACE "{"
#define CLOSE_BRACE "}"
#define COMMENT "//"
#define CLOSE_P ")"
#define CITATION_MARK "\""
#define APOSTOPHE "'"
#define SEMICOLON ";"
#define FOR "for"
#define CODE_DELIMETERS " (){}:<>*&|^/[];!?"
#define M_KEY 0xD65477

#include <iostream>
#include <list>
#include <cstring>
#include <string>
#include <queue>
#include <map>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "File.h"
#include "Sem.h"
#include "Message.h"

using namespace std;

class Just
{
    //Structures
private:
    struct argStruct{
        int errorCode;
        int e;
        bool isHelpAsked;
        list<string> inputFileNames;

        inline argStruct():errorCode(0), e(DEFAULT_E), isHelpAsked(false), inputFileNames(){}
    };

    struct recivedMessage{
        int counter;
        char word[M];

        inline recivedMessage():counter(0), word{0}{}
    };

    struct shmStruct{
        int rMNumber;
        recivedMessage rMArray[84];
    };

    //Fields
private:
    int pCounter;
    File file;
    argStruct argS;
    map<string, int> rWStructure;
    //Structure to store the multiple message from the sons is missing

    //Const fields
    const string help = "Usage: ./just [options] file\n"
                        "Options:\n"
                        "-i <input file>                Indicates the input file path.\n"
                        "-e <spaces>                    Indicate the number of spaces wanted to indent.\n"
                        "--help                         Show this message.\n"
                        "\n"
                        "None of this options are obligatory.\n"
                        "The program will expect at least one input file, each input file can be pass as follow:\n"
                        "$ ./just -i <inputFilePath 1> -i <inputFilePath 2> ... -i <inputFilePath N>\n"
                        "$ ./just <inputFilePath 1> <inputFilePath 2> ... <inputFilePath N>\n"
                        "Or they can be pass as a combination of both formats.";
    const string error = "Error: Invalid arguments";

    //Methods
public:
    Just();
    ~Just();
    void justify(int argc, char* argv[]);
    //Separate the line string into tokens.
    static queue<string> tokenize(const string &str, const char* delimeters);
private:
    string getOutputFileName(const string& iFName);
    void indent(const string& iFName, const string &oFName, Message &m ,int sonID);
    list<string> splitInstructions(string &line);
    //Indent an instruction.
    void indentInstruction(list<string> *justList, string instruction);
    //Delete all the spaces at the begining of a line.
    void deleteSpaces(string& str);
    //Add n spaces at the begining of line.
    void addSpaces(string& str, int n);
    //Check if there is a reserved word in instruction and update it's counter.
    void countReservedWords(const string &instruction);
    //Find the fist character of str in line that is not inside a comment or between " or '.
    size_t findFirstValidOf(const string& line, const string& str, size_t pos = 0);
    //Find the fist occurrence of str in line that is not inside a comment or between " or '.
    size_t findValid(const string& line, const string& str, size_t pos = 0);
    //Replace a comment with this form "//..." with NULL_C.
    void replaceComments(string& str);
    //Replace anyting between c in str with NULL_C.
    void replaceBetweenChar(string& str, const string& c);
    //"Initialize" argS based on the arguments past by the user.
    void initializeArguments(int argc, char *argv[]);
    void printHelp();
    void printError();
    void sendReservedWordData(Message &m, int mtype);
};

#endif // JUST_H
