#include <iostream>
#include <thread>
#include <dirent.h>
//#include "word2vec.c"

using namespace std;

bool _cout = 1;

void _temp_print(string _s, int _i1 = -1, int _i2 = -1){
    if( _i1!= -1) cout <<"["<< _i1 <<"/"<<_i2<<"]";
    cout << _s;
    cout << "\e[A";
    cout << "\33[2K\r";
}

void read_dataset(){
    for (size_t i = 0; i < 1<<20; i++);
}

int main(int argc, char const *argv[]){
    if(argc != 2){
        cout << "Modo de uso: "<< argv[0]<<" \"Nombre directorio\"" <<endl;
        return 1;
    }
    string path = argv[1];
    int num_files = 0;
    if (auto dir = opendir(path.c_str())) {
        while (auto f = readdir(dir)){
            if (!f->d_name || f->d_name[0] == '.') continue;
            num_files++;
        }
        closedir(dir);
    }
    if (auto dir = opendir(path.c_str())) {
        int _cont = 0;
        while (auto f = readdir(dir)){
            if (!f->d_name || f->d_name[0] == '.') continue;
            if(_cout)_temp_print("\n", _cont, num_files);
            _cont++;
            read_dataset();
        }
        closedir(dir);
    }
    cout <<"Total de archivos "<< num_files << endl;
    return 0;
}
