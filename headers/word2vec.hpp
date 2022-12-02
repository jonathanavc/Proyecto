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
    long long a, b, len;
    char c[max_w];
    for (b = 0; b < words; b++) {
        std::string sword;
        std::vector<float> M(size);
        a = 0;
        
        while (1) {
            c[a] = fgetc(f);
            sword.push_back(c[a]);
            if (feof(f) || (c[a] == ' ')) break;
            if ((a < max_w) && (c[a] != '\n')) a++;
        }
        c[a] = 0;
        for (a = 0; a < size; a++) fread(&M[a], sizeof(float), 1, f);
        std::transform(sword.begin(), sword.end(), sword.begin(), [](unsigned char c){ return std::toupper(c); });
        w2v.insert({c, M});
    }
    fclose(f);
    /*
    for (auto i = w2v.begin(); i != w2v.end(); i++){
        std::cout << i->first << ":";
        std::cout << i->second.size()<<std::endl;
        for (auto j = i->second.begin(); j != i->second.end(); i++){
            std::cout << *j<<" ";
        }
        std::cout << std::endl;
    }*/
    
}

word2vec::~word2vec(){
}

std::vector<float> * word2vec::getvec(std::string word){
    word.pop_back();
    if(w2v.find(word) == w2v.end()) return NULL;
    std::cout << "*******************";
    return &w2v[word];
}