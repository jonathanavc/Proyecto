#include <iostream>
#include <iomanip>
#include "mytime.hpp"

void temp_print(std::string _s, int _i1 = -1, int _i2 = -1, mytime * _mytime = NULL){
    double percent;
    if( _i1!= -1){
        percent = (double)(_i1 * 100) / _i2;
        std::cout << std::fixed << std::setprecision(2) <<"["<<percent<< "%]";
    }
    std::cout << _s;
    if(_mytime!=NULL){
        double tnecrep = 100.0 - percent;
        size_t dif = _mytime->getdif();
        std::cout << std::fixed << std::setprecision(2) << ((tnecrep * dif)/percent)/1000.0<<"s";
    }
    std::cout << "\n";
    std::cout << "\33[2K\r";
    std::cout << "\e[A";
}