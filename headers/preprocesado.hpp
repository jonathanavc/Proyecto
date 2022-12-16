#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <regex>
#include <string>
#include <unordered_set>
#include "porter2_stemmer/porter2_stemmer.cpp"

class preprocesado{
private:
  std::unordered_set<std::string> stopwords;
public:
  preprocesado();
  ~preprocesado();
  std::string preprocess_str(std::string);
};


preprocesado::preprocesado(){
  std::ifstream sw("stopwords.txt");
  std::string word;
  std::unordered_set<std::string> wd;
  while(sw >> word) wd.insert(word);
  stopwords = wd;
}

preprocesado::~preprocesado(){
}

std::string preprocesado::preprocess_str(std::string text){
  transform(text.begin(), text.end(), text.begin(), 
      [](unsigned char c){ return std::tolower(c); });
  std::regex e(".|\\.|,|:|;|'|\\*|\\+|=|\\?|");
  std::regex_replace(text, e, " ");
  std::string new_text;
  std::istringstream iss(text);
  do{
    std::string word;
    iss >> word;
    if(!stopwords.count(word)){
      Porter2Stemmer::trim(word);
      Porter2Stemmer::stem(word);
      new_text = new_text + word + " ";
    }
  }while(iss);
  return new_text;
}
