#include <string>
#include <unordered_set>
// 1. lowercase 2. puntuacion 3. stopwords 4. stemming

class preprocess{
private:
  std::unordered_set<std::string> stopwords;
public:
  preprocess();
  ~preprocess();
  std::string preprocess_str(std::string);
};
