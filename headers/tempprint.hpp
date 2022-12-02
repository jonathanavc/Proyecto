#include <iostream>

void temp_print(std::string _s, int _i1 = -1, int _i2 = -1){
    if( _i1!= -1){
        int percent = (_i1 * 100) / _i2;
        std::cout <<"["<<percent<< "%]";
    }
    std::cout << _s;
    std::cout << "\33[2K\r";
    std::cout << "\e[A";
}