#include <iostream>
#include <thread>
#include <dirent.h>
//#include "word2vec.c"

using namespace std;

bool _cout = 1;

void _temp_print(string _s){
    cout << _s;
    cout << "\e[A";
    cout << "\33[2K\r";
}

void read_dataset(){
    for (size_t i = 0; i < 1<<18; i++);
}

int main(int argc, char const *argv[]){
    if(argc != 2){
        cout << "Modo de uso: "<< argv[0]<<" \"Nombre directorio\"" <<endl;
        return 1;
    }
    int num_files = 0;
    string path = argv[1];
    
    if (auto dir = opendir(path.c_str())) {
        while (auto f = readdir(dir)){
            if (!f->d_name || f->d_name[0] == '.') continue;
            if(_cout)_temp_print((string)f->d_name + "\n");
            num_files++;
            read_dataset();
        }
        closedir(dir);
    }
    cout <<"Total de archivos "<< num_files << endl;
    return 0;
}
