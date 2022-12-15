#include <vector>
#include <mutex>
#include <string>

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


