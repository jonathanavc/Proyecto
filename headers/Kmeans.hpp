#include <algorithm>
#include <omp.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include <mutex>
#include <sstream>
#include <float.h>
#include <numeric>
#include "tempprint.hpp"

using namespace std;

struct Point{
  int pointID, clusterID;
  vector<double> components;

  Point(int id, const vector<double> &coords) 
    : pointID(id), components(coords) { clusterID = -1; }
};

struct Cluster{
  int clusterID;
  Point centroid;
  vector<Point> points;

  Cluster(int id, Point centroid)
    : clusterID(id), centroid(centroid){}

  void addPoint(Point &p){
    p.clusterID = this->clusterID;
    points.push_back(p);
  }
};

class Kmeans{
private:
  bool _cout;
  int K, iterations, n_threads;
  mutex *mutex_clusters;
  vector<Cluster> clusters;

  void setInitialPoints(vector<Point> &);
  int getNearestClusterID(Point);

public:
  Kmeans(int, int, int, bool);
  void run(vector<Point> &); 
  void writeResults(string);
};

void Kmeans::setInitialPoints(vector<Point> &all_points){
  set<int> used_point_ids;
  for(int i = 0, index; i < K; i++){
    do{ index = rand() % all_points.size();
    } while(used_point_ids.count(index));
    used_point_ids.insert(index);
    Cluster cluster(i, all_points[index]);
    cluster.addPoint(all_points[index]);
    clusters.push_back(cluster);
  }
}

// Recorrer centroides para encontrar el cluster mas cercanos a un punto
int Kmeans::getNearestClusterID(Point point) {
  double min_dist = DBL_MAX, dist;
  int NearestClusterID = -1;
  for(Cluster &cluster : clusters){
    vector<double> tmp(cluster.centroid.components.size());
    transform(cluster.centroid.components.begin(), cluster.centroid.components.end(), 
        point.components.begin(), tmp.begin(),
        [](auto a, auto b) -> double {return (a-b) * (a-b);});
    dist = sqrt(accumulate(tmp.begin(), tmp.end(), 0));
    if(dist < min_dist){ min_dist = dist; NearestClusterID = cluster.clusterID; }
  }
  return NearestClusterID;
}

Kmeans::Kmeans(int num_clusters, int max_iterations, int nthreads, bool _cout = true) 
  : K(num_clusters), iterations(max_iterations), n_threads(nthreads), _cout(_cout){
    mutex_clusters = new mutex[n_threads];
  }  

void Kmeans::run(vector<Point> &all_points) {
  int dimensions = all_points[0].components.size();

  setInitialPoints(all_points);

  if(_cout) cout << "Numero de Clusters = " << clusters.size()  << endl;
  if(_cout) cout << "Dimensión de cada punto = " << dimensions << endl;
  if(_cout) cout << "Cantidad de puntos = " << all_points.size() << endl;
  if(_cout) temp_print("Running K-Means Clustering..");

  int iter = 1;
  for(bool done = true; iter <= iterations; iter++, done = true){
    if(_cout) temp_print("Iteracion " + to_string(iter) +" de "+ to_string(iterations), 0, 4);

    //temp_print("Iter",iter,iterations);
    int all_points_size = all_points.size();
    int conv = 0;
    // Add all points to their nearest cluster
    #pragma omp parallel for reduction(+: conv) num_threads(n_threads)
    for(int i = 0; i < all_points_size; i++){
      int nearestClusterID = getNearestClusterID(all_points[i]);
      if(all_points[i].clusterID == nearestClusterID) continue;
      // Cambiar cluster_id de Point
      all_points[i].clusterID = nearestClusterID;
      conv++;
    }
    if(conv!= 0) done = false;
    if(_cout) temp_print("Convergencia en Iteracion " + to_string(iter) +" de "+ to_string(iterations), conv, all_points_size, false);
    if(_cout) temp_print("Iteracion " + to_string(iter) +" de "+ to_string(iterations), 1, 4);

    // Si converge termina el ciclo
    if(done) break;
    
    // Se limpian los clusters
    #pragma omp parallel for num_threads(n_threads)
    for(Cluster& cluster : clusters){
      cluster.points.clear();
    }
    if(_cout) temp_print("Iteracion " + to_string(iter) +" de "+ to_string(iterations), 2, 4);

    // Se agregan los puntos a su nuevo cluster
    // mejorará esto en paralelo?
    #pragma omp parallel for num_threads(n_threads)
    for (int i = 0; i < all_points_size; i++){
      int clusterID = all_points[i].clusterID;
      mutex_clusters[clusterID].lock();
      clusters[clusterID].addPoint(all_points[i]);
      mutex_clusters[clusterID].unlock();
    }
    if(_cout) temp_print("Iteracion " + to_string(iter) +" de "+ to_string(iterations), 3, 4);

    // Recalculating the center of each cluster
    for(Cluster &cluster : clusters){
      // ocurre en algun caso ????
      if(cluster.points.size() <= 0) continue;
      // Promedio por dimension 
      #pragma omp parallel for num_threads(n_threads) //esto si q si 😎
      for(int i = 0; i < dimensions; i++){
        double sum = 0.0;
        //#pragma omp parallel for reduction(+: sum) num_threads(n_threads) //no funciona tan bien
        for(Point &point : cluster.points) sum += point.components[i];
        cluster.centroid.components[i] = (sum / cluster.points.size());
      }
    }
    if(_cout) temp_print("Iteracion " + to_string(iter) +" de "+ to_string(iterations), 4, 4);
  }
  if(_cout) cout << "Clustering completed in iteration: " << iter - 1 << endl;
  for(Cluster cluster : clusters){
    if(_cout) cout<<"Cluster: "<<cluster.clusterID << endl;; //add centroid document
    if(_cout) cout<<"Elementos: " << cluster.points.size() << endl;
  }
}

void Kmeans::writeResults(string output_dir){
  ofstream pointsFile;
  pointsFile.open(output_dir + "/" + to_string(K) + "-points.txt", ios::out);
  for(Cluster cluster : clusters)
    for(Point point : cluster.points)
      pointsFile<<point.clusterID<<":"<<point.pointID<<endl;
  pointsFile.close();

  // Write cluster centers to file
  ofstream outfile;
  outfile.open(output_dir + "/" + to_string(K) + "-clusters.txt");
  if(!outfile.is_open()){ if(_cout) cout<<"Error: Unable to write to clusters.txt"; return; }
  for(Cluster cluster : clusters){
    for(double component : cluster.centroid.components)
      outfile<<component<<" ";
    outfile<<endl;
  }
  outfile.close();
}
