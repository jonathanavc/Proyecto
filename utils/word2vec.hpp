#include <iostream>
#include <vector>
#include <map>
#include "tempprint.hpp"

const long long max_w = 50;  

class word2vec{
private:
    std::map<std::string, int > w2v;
    long long words, size;
    float * M;
    bool _cout;
public:
    word2vec(std::string file_name, bool __cout);
    ~word2vec();
    float * getvec(std::string word);
    long long getdim();
};

word2vec::word2vec(std::string file_name, bool __cout = 1){
    FILE * f = fopen(file_name.c_str(), "rb");
    mytime _mytime;
    _cout = __cout;
    if (f == NULL) {
        printf("Input file not found\n");
        return;
    }
    fscanf(f, "%lld", &words);
    fscanf(f, "%lld", &size);
    long long a, b, len;
    char c[max_w];
    M = (float *) malloc(size * words * sizeof(float));
    int cont = 0;
    _mytime.start("");
    if(_cout) temp_print("cargando word2vec...");
    for (b = 0; b < words; b++) {
        if(_cout && cont%10000 == 0)temp_print("Cargando w2v... tiempo restante:",cont, words, &_mytime);
        std::string sword;
        a = 0;
        while (1) {
            c[a] = fgetc(f);
            sword.push_back(c[a]);
            if (feof(f) || (c[a] == ' ')) break;
            if ((a < max_w) && (c[a] != '\n')) a++;
        }
        c[a] = 0;
        for (a = 0; a < size; a++) fread(&M[size * cont + a], sizeof(float), 1, f);
        std::transform(sword.begin(), sword.end(), sword.begin(), [](unsigned char c){ return std::tolower(c); });
        w2v.insert({c, cont});
        cont++;
    }
    fclose(f);
    if(_cout) temp_print("Word2vec cargado en ",-1,-1, &_mytime, false);
}

word2vec::~word2vec(){
    delete M;
}

float * word2vec::getvec(std::string word){
    word.pop_back();
    if(w2v.find(word) == w2v.end()) return NULL;
    return M+(size*w2v[word]);
}

long long word2vec::getdim(){
    return size;
}

