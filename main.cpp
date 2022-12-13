#include <iostream>
#include <thread>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <mutex>
#include "headers/json.hpp"
#include "headers/Kmeans.hpp"
#include "headers/word2vec.hpp"
#include "headers/preprocesado.hpp"

using namespace std;

string word2vec_file = "GoogleNews-vectors-negative300.bin";
int w2v_dim = 0;
word2vec * w2v;
bool _cout = 1;
vector<Point> points;
mutex mtx;

void read_dataset(string dir){
    preprocesado _pp;
    ifstream dataset_file(dir);
    stringstream buffer;
    buffer << dataset_file.rdbuf();
    auto json_file = nlohmann::json::parse(buffer);
    for (auto text : json_file){
        int _id = atoi(((string)text["id"]).c_str());
        string _text(text["text"].get<std::string>());
        _text = _pp.preprocess_str(_text);
        stringstream words(_text);
        string word;
        float * M;
        //valores en 0
        vector<double> resumen(w2v_dim, 0.0);
        int words_count = 0;
        while (words >> word) {
            cout << word << endl;
            word.push_back(0);
            M = w2v->getvec(word);
            //agregar al resumen
            if(M != NULL) {
                for (size_t i = 0; i < w2v_dim; i++){
                    resumen[i] += M[i];
                }
                words_count++;
            }
        }
        for (size_t i = 0; i < w2v_dim; i++) resumen[i] /= words_count;
        cout << _text.size() << endl;
        cout << words_count << endl;
        for (size_t i = 0; i < 5; i++)
            cout << resumen[i] <<" ";
        cout<<endl;
        mtx.lock();
        points.push_back(Point(_id, resumen));
        mtx.unlock();
    }
}

int main(int argc, char const *argv[]){
    srand(time(NULL));

    if(argc != 3){
        cout << "Modo de uso: "<< argv[0]<<" \"Nombre directorio\" \"N°threads\"" <<endl;
        return 1;
    }

    string path = argv[1];
    int n_threads = atoi(argv[2]);
    if(n_threads < 0) n_threads = 1;
    thread threads[n_threads];
    int num_files = 0;
    mytime _mytime;

    if (auto dir = opendir(path.c_str())) {
        while (auto f = readdir(dir)){
            if (!f->d_name || f->d_name[0] == '.') continue;
            num_files++;
        }
        closedir(dir);
    }

    if (auto dir = opendir(path.c_str())) {
        w2v = new word2vec(word2vec_file, _cout);
        w2v_dim = w2v->getdim();
        int _cont = 0;
        int _cont_fin = 0;
        _mytime.start("");
        if(_cout)temp_print("Leyendo dataset...");
        while (auto f = readdir(dir)){
            if (!f->d_name || f->d_name[0] == '.') continue;
            if(threads[_cont % n_threads].joinable()){
                _cont_fin++;
                threads[_cont % n_threads].join();
                if(_cout)temp_print("Leyendo dataset... tiempo restante: ", _cont, num_files, &_mytime);
            }
            threads[_cont % n_threads] = thread(&read_dataset, path + + f->d_name);
            _cont++;
        }
        for (int i = 0; i < n_threads;i++){
            if(threads[i].joinable()) threads[i].join();
        }
        closedir(dir);
    }
    if(_cout)temp_print("Ejecutando KMEANS...");
    Kmeans kmeans(10, 5, n_threads);
    kmeans.run(points);
    kmeans.writeResults("output");
    return 0;
}
