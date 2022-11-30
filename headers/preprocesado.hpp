#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <unordered_set>
#include "english_stem.h"

using namespace std;

// 1. lowercase 2. puntuacion 3. stopwords 4. stemming

class preprocesado {
    private:
        unordered_set<string> stopwords;
    public:
        preprocesado(){
          unordered_set<string> wd{
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
        ~preprocesado(){
        }

        string preprocess_str(string text){
          transform(text.begin(), text.end(), text.begin(), [](unsigned char c){ return std::tolower(c); });
          regex e(".|,|:|;|'|");
          regex_replace(text, e, "");
          string new_text;
          istringstream iss(text);
          do{
            string word;
            iss >> word;
            if(!stopwords.count(word)) 
              new_text = new_text + word;
          }while(iss);
          regex_replace(text, e, "");
          stemming::english_stem<> StemEnglish;
          StemEnglish(text);
          return text;
        }
    
};
