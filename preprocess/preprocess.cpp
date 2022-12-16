#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <regex>
#include "preprocess.hpp"
#include "porter2_stemmer/porter2_stemmer.cpp"

preprocess::preprocess(){
  std::ifstream sw("stopwords.txt");
  std::string word;
  std::unordered_set<std::string> wd;
  while(sw >> word) wd.insert(word);
  stopwords = wd;
}

preprocess::~preprocess(){
}

std::string preprocess::preprocess_str(std::string text){
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
