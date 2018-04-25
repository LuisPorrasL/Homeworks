#ifndef JUST_H
#define JUST_H
#define M 84
#define NULL_C '*'

#include<list>
#include<cstring>
#include<string>
#include<queue>

using namespace std;

//Gobal variables
static const string rWords[] = {"alignas", "aligof", "and", "and_eq", "asm", "auto", "bitand", "bitor", "bool", "break", "case",
                                "catch", "char", "char16_t", "char32_t", "class", "compl", "const", "constexpr", "const_cast",
                                "continue", "decltype", "default", "delete", "do", "double", "dynamic_cast", "else", "enum",
                                "explicit", "export", "extern", "false", "float", "for", "friend", "goto", "if", "inline", "int",
                                "long", "mutable", "namesapace", "new", "noexept", "not", "not_eq", "nullptr", "operator", "or",
                                "or_equ", "private", "protected", "public", "register", "reinterpret_cast", "return", "short",
                                "signed", "sizeof", "static", "static_assert", "static_cast", "struct", "switch", "template",
                                "this", "thread_local", "throw", "true", "try", "typedef", "typeid", "typename", "union", "unsigned",
                                "using", "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq"};

class Just
{
    //Constantes
private:
    static const char* rWDelimeters;
    static const string spaces;
    static const string encabesadoRWD;

private:
    int e;
    int pCounter; //Count the number of open parenthesis {, is use to keep track of the right identation to use.
    int* rWCounter;

public:
    Just();
    ~Just();
    //Separate each line of code(string from fList) into individual instructions and indents them.
    list<string>justify(list<string> fList, int userE);
    //Create a std::list of std::string, each string being a pair of a reserved word and it's respective counter.
    list<string>convertReservedWordsData(bool all = false);
    //Separate the line string into tokens.
    static queue<string> tokenize(const string &line, const char* delimeters);

private:
    //Find the fist character of str in line that is not inside a comment or between " or '.
    size_t findValidFirstOf(string line, const string &str, size_t pos = 0);

    //Replace a comment with this form "//..." with NULL_C
    void replaceComment(string& str);

    //Replace anyting between charR to NULL_C in str
    void replaceCharRange(string& str, const string& charR);

    //Indent an instruction.
    void indentInst(string &inst, list<string> *jF);

    //Check if there is a reserved word in str and update it's counter.
    void countReservedWords(string str);

    //Compare str to the reserved words of the rWords array and return it's index if found, otherwise return -1.
    int searchReservedWord(const string& str);

    //Delete all the spaces at the begining of a line.
    void deleteSpaces(string& str);

    //Add n spaces at the begining of line.
    void addSpaces(string& str, int n);
};

#endif // JUST_H
