#include "File.h"

File::File(){}

list<string> File::read(string fName){

    list<string> readFile;
    string line;
    istream* in;
    ifstream file(fName);
    if(file.is_open()){
        in = &file;
    }
    else if(fName == "std::cin"){
        in = &cin;
    }
    else{
        cout << errorMessage << '\n';
        return readFile;
    }

    while (getline (*in, line)){
        readFile.push_back(line);
    }
    file.close();

    return readFile;
}

void File::write(const list<string>& fList, string fName){

    if(!fList.empty()){
        ostream* out;
        ofstream file;

        if(fName == "std::cout"){
            out = &cout;
        }
        else{
            file.open(fName);
            if(file.is_open()){
                out = &file;
            }
            else{
                cout << errorMessage << '\n';
                return;
            }
        }
        for(list<string>::const_iterator ind = fList.begin(); ind != fList.end(); ++ind){
            *out << *ind << '\n';
        }
        file.close();
    }
}
