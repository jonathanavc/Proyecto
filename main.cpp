#include <iostream>
#include <thread>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <mutex>
#include "utils/json.hpp"
#include "utils/word2vec.hpp"
#include "kmeans/kmeans.cpp"
#include "preprocess/preprocess.cpp"

using namespace std;


mutex mtx;
const string word2vec_file = "GoogleNews-vectors-negative300.bin";
bool _cout = 1;
int w2v_dim = 0;

word2vec * w2v;
vector<Point> points;

void read_dataset(string dir){
    preprocess _pp;
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
        //for (size_t i = 0; i < w2v_dim; i++) resumen[i] /= words_count;
        mtx.lock();
        points.push_back(Point(_id, resumen));
        mtx.unlock();
    }
}

int main(int argc, char const *argv[]){
    srand(time(NULL));
    if(argc != 5){
        cout << "Modo de uso: "<< argv[0]<<" \"Nombre directorio\"  \"NumClusters\"  \"MaxIter\"  \"N°threads\"" <<endl;
        return 1;
    }

    string path = argv[1];
    int K = atoi(argv[2]);
    int MaxIter = atoi(argv[3]);
    int n_threads = atoi(argv[4]);
    if(K < 0) K = 1;
    if(MaxIter < 0) MaxIter = 1;
    if(n_threads < 0) n_threads = 1;
    
    thread threads[n_threads];
    
    mytime _mytime;
    int num_files = 0;
    if (auto dir = opendir(path.c_str())) {
        while (auto f = readdir(dir)){
            if (!f->d_name || f->d_name[0] == '.') continue;
            num_files++;
        }
        closedir(dir);
    }

    // Leer archivos en paralelo
    if (auto dir = opendir(path.c_str())) {
        // Cargar w2v
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
                if(_cout && _cont % n_threads == n_threads - 1)temp_print("Leyendo dataset... tiempo restante: ", _cont, num_files, &_mytime);
            }
            threads[_cont % n_threads] = thread(&read_dataset, path + + f->d_name);
            _cont++;
        }
        for (int i = 0; i < n_threads;i++){
            if(threads[i].joinable()) threads[i].join();
        }
        closedir(dir);
        if(_cout)temp_print("Dataset procesado en ",-1,-1, &_mytime, false);
    }
    
    
    // Ejecutar Kmeans
    Kmeans kmeans(K, MaxIter, n_threads, _cout);
    _mytime.start("");
    kmeans.run(points);
    if(_cout) temp_print("Kmeans ejecutado en ",-1,-1, &_mytime, false);
    // Guardar Resultados de Kmeans
    _mytime.start("");
    if(_cout) temp_print("Guardando resultados en output...");
    kmeans.writeResults("output");
    if(_cout) temp_print("Resultados guardados en ",-1,-1, &_mytime, false);
    return 0;
}
