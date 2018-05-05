#ifndef JUST_H
#define JUST_H
#define DEFAULT_E 4
#define NULL_C '*'
#define SPACE ' '
#define SHM_KEY 0xD65477
#define RMA_M 100

#include <iostream>
#include <list>
#include <cstring>
#include <string>
#include <queue>
#include <map>
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
        recivedMessage rMArray[RMA_M];

        inline shmStruct():rMNumber(0), rMArray(){}
    };

    //Const fields
private:
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
    const string spaces = " \t";
    const string instructionDelimeters = "{};";
    const string braces = "{}";
    const string openBrace = "{";
    const string closeBrace = "}";
    const string comment = "//";
    const string closeP = ")";
    const string citationMark = "\"";
    const string apostrophe = "'";
    const string semicolon = ";";
    const string forS = "for";
    const string codeDelimeters = " (){}:<>*&|^/[];!?";

    //Fields
private:
    int pCounter;
    File file;
    argStruct argS;
    map<string, int> rWStructure;

    //Methods
public:
    Just();
    ~Just();
    //Indent multiple files passed by arguments (argc, argv) and print the sum of the reserved word data of each file.
    void justify(int argc, char* argv[]);
    //Separate the line string into tokens.
    static queue<string> tokenize(const string &str, const char* delimeters);
private:
    //Generate an output filename based on the input filename or path (iFName)
    string getOutputFileName(const string& iFName);
    //Indent the code of the file with path iFName.
    void indent(const string& iFName, const string &oFName, Message &m ,int sonID);
    //Separate line into sigle code instructions.
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
    //Print a help message for the user.
    void printHelp();
    //Print an error message for the user.
    void printError();
    //Send the data of each reserved word (word and counter) on rWStructure to m.
    void sendReservedWordData(Message &m, int mtype);
    //Return the number of diferent fist letters of the reserved words.
    int countReservedWordsFistLetters();
};

#endif // JUST_H
