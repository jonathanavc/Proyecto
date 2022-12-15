all: execute

execute: kmeans.o preprocess.o porter2_stemmer.o
	g++ -std=c++17 -pthread -fopenmp -O3 -o execute main.cpp kmeans.o preprocess.o porter2_stemmer.o

kmeans.o: kmeans/kmeans.cpp kmeans/kmeans.hpp
	g++ -c -std=c++17 -pthread  -fopenmp -O3 kmeans/kmeans.cpp

preprocess.o: preprocess/preprocess.cpp preprocess/preprocess.hpp porter2_stemmer.o
	g++ -c -std=c++17 -O3 -pthread preprocess/preprocess.cpp 

porter2_stemmer.o: preprocess/porter2_stemmer/porter2_stemmer.cpp preprocess/porter2_stemmer/porter2_stemmer.h
	g++ -c -std=c++17 -pedantic -O3 -pthread preprocess/porter2_stemmer/porter2_stemmer.cpp
