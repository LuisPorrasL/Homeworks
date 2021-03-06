#ifndef FILE_H
#define FILE_H

#include <list>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

//Constantes
const string errorMessage = "Unable to open file";

class File
{
public:
    File();
    //Open the file with name fName and read it, if fName is "std::cin" it will read from the standar input.
    list<string> read(string fName);
    //Open or creates a file with name fName and write on it, if fName is "std::cout" it will write on the standar output.
    void write(const list<string> &fList, string fName);
};

#endif // FILE_H
