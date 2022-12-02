#include <iostream>
#include <iomanip>

void temp_print(std::string _s, int _i1 = -1, int _i2 = -1){
    if( _i1!= -1){
        float percent = (float)(_i1 * 100) / _i2;
        std::cout << std::fixed << std::setprecision(2) <<"["<<percent<< "%]";
    }
    std::cout << _s;
    std::cout << "\n";
    std::cout << "\33[2K\r";
    std::cout << "\e[A";
}