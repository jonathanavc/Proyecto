#include <bits/stdc++.h>

class mytime {
 private:
  std::string _s;
  std::chrono::_V2::system_clock::time_point _start;

 public:
  mytime(){}
  ~mytime(){}
  void start(std::string _s) {
    this->_s = _s;
    _start = std::chrono::system_clock::now();
  }
  void end() {
    std::cout << _s << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - _start).count() << " ms" << std::endl;
  }
  /**
   * @return Tiempo que ha pasado desde start() en ms's.
   */
  size_t getdif(){
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - _start).count();
  }
};