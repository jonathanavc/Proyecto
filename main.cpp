#include <iostream>
#include <thread>
#include <dirent.h>
#include <fstream>
#include <sstream>
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

void read_dataset(string dir){
    ifstream dataset_file(dir);
    stringstream buffer;
    buffer << dataset_file.rdbuf();
    auto json_file = nlohmann::json::parse(buffer);
    for (auto text : json_file){
        preprocesado * _pp = new preprocesado();
        int _id = atoi(((string)text["id"]).c_str());
        string _text = text["text"];
        _text = _pp->preprocess_str(_text);
        std::stringstream words(_text);
        string word;
        float * M;
        //valores en 0
        //float resumen[w2v_dim]{};
        vector<double> resumen(w2v_dim, 0.0);
        int words_count = 0;
        while (words >> word) {
            M = w2v->getvec(word);
            //agregar al resumen
            if(M != NULL) {
                for (size_t i = 0; i < w2v_dim; i++){
                    resumen[i] += M[i];
                    words_count++;
                }
            }
        }
        for (size_t i = 0; i < w2v_dim; i++) resumen[i] /= words_count;
        
        points.push_back(Point(_id, resumen));
    }
}

int main(int argc, char const *argv[]){
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
    Kmeans kmeans(10, 5, n_threads);
    kmeans.run(points);
    kmeans.writeResults("output");
    return 0;
}
