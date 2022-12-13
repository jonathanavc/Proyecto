#include <algorithm>
#include <omp.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include <sstream>
#include <float.h>
#include <numeric>

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
  int K, iterations;
  vector<Cluster> clusters;

  void setInitialPoints(vector<Point> &);
  int getNearestClusterID(Point);

public:
  Kmeans(int, int, int);
  void run(vector<Point> &); 
  void writeResults(string);
};

void Kmeans::setInitialPoints(vector<Point> &all_points){
  set<int> used_point_ids;
  for(int i = 0, index; i < K; i++){
    do{ index = rand() % all_points.size();
    } while(used_point_ids.count(index));
    used_point_ids.insert(index);
    all_points[index].clusterID = i;
    Cluster cluster(i, all_points[index]);
    clusters.push_back(cluster);
  }
}

// Recorrer centroides para encontrar el cluster mas cercanos a un punto
int Kmeans::getNearestClusterID(Point point) {
  double min_dist = DBL_MAX, dist;
  int NearestClusterID = -1;
  for(Cluster &cluster : clusters){
    vector<double> tmp;
    transform(cluster.centroid.components.begin(), cluster.centroid.components.end(), 
        point.components.begin(), tmp.begin(),
        [](auto a, auto b) -> double {return (a+b) * (a+b);});
    dist = sqrt(accumulate(tmp.begin(), tmp.end(), 0));
    if(dist < min_dist){ min_dist = dist; NearestClusterID = cluster.clusterID; }
  }
  return NearestClusterID;
}

Kmeans::Kmeans(int num_clusters, int max_iterations, int nthreads) 
  : K(num_clusters), iterations(max_iterations){}  

void Kmeans::run(vector<Point> &all_points) {
  int dimensions = all_points[0].components.size();

  setInitialPoints(all_points);

  cout << "Clusters initialized = " << clusters.size() << endl << endl;
  cout << "Points with dimension = " << dimensions << endl << endl;
  cout << "Running K-Means Clustering.." << endl;

  int iter = 1;
  for(bool done = true; (iter <= iterations) && !done; iter++){
    cout << "Iter - " << iter << "/" << iterations << endl;

    // Add all points to their nearest cluster
    #pragma omp parallel for reduction(&&: done) num_threads(nthreads)
    for(Point &point : all_points){
      int nearestClusterID = getNearestClusterID(point);
      if(point.clusterID == nearestClusterID) continue;

      // Se elimina el punto actual de su cluster antiguo
      clusters[point.clusterID].points.erase(
          remove(clusters[point.clusterID].points.begin(), clusters[point.clusterID].points.end(), point));

      // Se agrega el punto a su cluster mas cercano. Su clusterID se actualiza
      clusters[nearestClusterID].addPoint(point);
      done = false;
    }
    
    // Recalculating the center of each cluster
    for(Cluster &cluster : clusters){
      // ocurre en algun caso ????
      if(cluster.points.size() <= 0) continue;
      // Promedio por dimension
      for(int i = 0; i < dimensions; i++){
        double sum = 0.0;
        #pragma omp parallel for reduction(+: sum) num_threads(nthreads)
        for(Point &point : cluster.points) sum += point.components[i];
        cluster.centroid.components[i] = (sum / cluster.points.size());
      }
    }
    done = true;
  }
  cout << "Clustering completed in iteration : " << iter << endl << endl;

  for(Cluster cluster : clusters){
    cout<<"Cluster: \n"; //add centroid document
    cout<<cluster.points.size()<<endl;
    for(Point point : cluster.points){
      for(double component : point.components)
        cout<<component<<" ";
      cout<<endl;
    }
    cout<<endl;
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
  if(!outfile.is_open()){ cout<<"Error: Unable to write to clusters.txt"; return; }
  for(Cluster cluster : clusters){
    for(double component : cluster.centroid.components)
      outfile<<component<<" ";
    outfile<<endl;
  }
  outfile.close();
}
