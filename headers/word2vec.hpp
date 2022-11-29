#include <iostream>
#include <vector>
#include <map>
#include <math.h>

const long long max_w = 50;  

class word2vec{
private:
    std::map<std::string,std::vector<float>> w2v;
    long long words, size;
public:
    word2vec(std::string file_name);
    ~word2vec();
    std::vector<float> * getvec(std::string word);
};

word2vec::word2vec(std::string file_name){
    FILE * f = fopen(file_name.c_str(), "rb");
    if (f == NULL) {
        printf("Input file not found\n");
        return;
    }
    fscanf(f, "%lld", &words);
    fscanf(f, "%lld", &size);
    std::map<std::string,std::vector<float>> w2v;
    long long a, b, len;
    char word[max_w];
    std::vector<float> M(size);
    for (b = 0; b < words; b++) {
        a = 0;
        //texto
        while (1) {
            word[a] = fgetc(f);
            if (feof(f) || (word[a] == ' ')) break;
            if ((a < max_w) && (word[a] != '\n')) a++;
        }
        word[a] = 0;
        for (a = 0; a < size; a++) fread(&M[a + b * size], sizeof(float), 1, f);
        len = 0;
        for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
        len = sqrt(len);
        for (a = 0; a < size; a++) M[a + b * size] /= len;
        w2v[(std::string)word] = M;
    }
    fclose(f);
}

word2vec::~word2vec(){
}

std::vector<float> * word2vec::getvec(std::string word){
    if(w2v.find(word) == w2v.end()) return NULL;
    else return &w2v[word];
}