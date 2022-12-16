#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <cstdlib>
#include <string>
#include <regex>
#include <unordered_set>
#include "pstem/stemming/english_stem.hpp"
// 1. lowercase 2. puntuacion 3. stopwords 4. stemming

class preprocesado {
private:
  std::string ws2s(const std::wstring& wstr){
    std::string str(wstr.length(), 0);
    std::transform(wstr.begin(), wstr.end(), str.begin(), [] (wchar_t c) {
        return (char)c;
    });
    return str;
  }

    std::unordered_set<std::string> stopwords;
public:
    preprocesado();
    ~preprocesado();
    std::string preprocess_str(std::string text);
};

preprocesado::preprocesado(){
  std::unordered_set<std::string> wd{
    "i", "me", "my", "myself", "we", "our", "ours", "ourselves", "you", "your", "yours", "yourself",
    "yourselves", "he", "him", "his", "himself", "she", "her", "hers", "herself", "it",
    "its", "itself", "they", "them", "their", "theirs", "themselves", "what", "which", "who",
    "whom", "this", "that", "these", "those", "am", "is", "are", "was", "were",
    "be", "been", "being", "have", "has", "had", "having", "do", "does", "did",
    "doing", "a", "an", "the", "and", "but", "if", "or", "because", "as",
    "until", "while", "of", "at", "by", "for", "with", "about", "against", "between",
    "into", "through", "during", "before", "after", "above", "below", "to", "from", "up",
    "down", "in", "out", "on", "off", "over", "under", "again", "further", "then",
    "once", "here", "there", "when", "where", "why", "how", "all", "any", "both",
    "each", "few", "more", "most", "other", "some", "such", "no", "nor", "not",
    "only", "own", "same", "so", "than", "too", "very", "s", "t", "can",
    "will", "just", "don", "should", "now" 
  };
  stopwords = wd;
}
preprocesado::~preprocesado(){
}

std::string preprocesado::preprocess_str(std::string text){
  transform(text.begin(), text.end(), text.begin(), [](unsigned char c){ return std::tolower(c); });
  std::regex e(".|,|:|;|'|");
  replace(text.begin(), text.end(), '=', ' ');
  replace(text.begin(), text.end(), '*', ' ');
  std::regex_replace(text, e, " ");
  std::string new_text;
  std::istringstream iss(text);
  do{
    std::string word;
    iss >> word;
    if(!stopwords.count(word)) 
      new_text = new_text + word + " ";
  }while(iss);
  stemming::english_stem<> StemEnglish;
  std::wstring w_new_text(new_text.begin(), new_text.end());
  StemEnglish(w_new_text);
  //std::wcout << w_new_text << std::endl;
  return ws2s(w_new_text);
  //return new_text;
}
