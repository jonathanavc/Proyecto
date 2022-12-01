#include <algorithm>
//#include <omp.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

struct Point{
  int pointId, clusterId, dimensions;
  vector<double> values;

  vector<double> lineToVec(string &line){
    vector<double> values;
    double val;
    istringstream iss(line);
    while(iss >> val)
      values.push_back(val);
    return values;
  }

  Point(int id, string line){
      pointId = id;
      values = lineToVec(line);
      dimensions = values.size();
      clusterId = 0; // Initially not assigned to any cluster
  }
};


struct Cluster{
  int clusterId;
  vector<double> centroid;
  vector<Point> points;

  Cluster(int clusterId, Point centroid){
    this->clusterId = clusterId;
    this->centroid = centroid.values;
  }

  void addPoint(Point p){
    p.clusterId = this->clusterId;
    points.push_back(p);
  }

  bool removePoint(int pointId){ 
    for(int i = 0; i < points.size(); i++)
      if(points[i].pointId == pointId){
        points.erase(points.begin() + i);
        return true;
      }
    return false;
  }
  double centroidComponent(int pos) { return centroid[pos]; }
};

class KMeans{
private:
  int K, iters, dimensions, total_points;
  vector<Cluster> clusters;
  string output_dir;

  void clearClusters() {
    for (int i = 0; i < K; i++)
      clusters[i].points.clear();
  }

  int getNearestClusterId(Point point) {
      double sum = 0.0, min_dist;
      int NearestClusterId;
      if(dimensions==1) 
          min_dist = abs(clusters[0].centroidComponent(0) - point.values[0]);
      else {
        for (int i = 0; i < dimensions; i++)
          sum += pow(clusters[0].centroidComponent(i) - point.values[i], 2.0);
           // sum += abs(clusters[0].centroidComponent(i) - point.getVal(i));
        min_dist = sqrt(sum);
      }
      NearestClusterId = clusters[0].clusterId;

      for(int i = 1; i < K; i++){
          double dist;
          sum = 0.0;
          if(dimensions==1) 
            dist = abs(clusters[i].centroidComponent(0) - point.values[0]);
          else{
            for (int j = 0; j < dimensions; j++)
              sum += pow(clusters[i].centroidComponent(j) - point.values[j], 2.0);
                // sum += abs(clusters[i].centroidComponent(j) - point.getVal(j));
            dist = sqrt(sum);
            // dist = sum;
          }
          if (dist < min_dist) {
              min_dist = dist;
              NearestClusterId = clusters[i].clusterId;
          }
      }
      return NearestClusterId;
  }

public:
  KMeans(int K, int iterations, string output_dir) {
      this->K = K;
      this->iters = iterations;
      this->output_dir = output_dir;
  }

  void run(vector<Point> &all_points) {
    total_points = all_points.size();
    dimensions = all_points[0].dimensions;

    // Initializing Clusters
    vector<int> used_pointIds;

    for (int i = 1; i <= K; i++) {
        while (true) {
            int index = rand() % total_points;

            if (find(used_pointIds.begin(), used_pointIds.end(), index) == used_pointIds.end()) {
                used_pointIds.push_back(index);
                all_points[index].clusterId = i;
                Cluster cluster(i, all_points[index]);
                clusters.push_back(cluster);
                break;
            }
        }
    }
    cout << "Clusters initialized = " << clusters.size() << endl << endl;
    cout << "Running K-Means Clustering.." << endl;

    int iter = 1;
    while (true) {
      cout << "Iter - " << iter << "/" << iters << endl;
      bool done = true;

      // Add all points to their nearest cluster
      #pragma omp parallel for reduction(&&: done) num_threads(16)
      for (int i = 0; i < total_points; i++) {
        int currentClusterId = all_points[i].clusterId;
        int nearestClusterId = getNearestClusterId(all_points[i]);

        if (currentClusterId != nearestClusterId) {
          all_points[i].clusterId = nearestClusterId;
          done = false;
        }
      }

      // clear all existing clusters
      clearClusters();

      // reassign points to their new clusters
      for (int i = 0; i < total_points; i++)
        // cluster index is ID-1
        clusters[all_points[i].clusterId - 1].addPoint(all_points[i]);

      // Recalculating the center of each cluster
      for (int i = 0; i < K; i++) {
        int ClusterSize = clusters[i].points.size();

        for (int j = 0; j < dimensions; j++) {
          double sum = 0.0;
          if (ClusterSize > 0) {
            #pragma omp parallel for reduction(+: sum) num_threads(16)
            for (int p = 0; p < ClusterSize; p++)
              sum += clusters[i].points[p].values[j];

            clusters[i].centroid[j] = (sum / ClusterSize);
          }
        }
      }

      if (done || iter >= iters) {
        cout << "Clustering completed in iteration : " << iter << endl << endl;
        break;
      }
      iter++;
    }

    ofstream pointsFile;
    pointsFile.open(output_dir + "/" + to_string(K) + "-points.txt", ios::out);

    for (int i = 0; i < total_points; i++) 
      pointsFile << all_points[i].clusterId << endl;

    pointsFile.close();

    // Write cluster centers to file
    ofstream outfile;
    outfile.open(output_dir + "/" + to_string(K) + "-clusters.txt");
    if (outfile.is_open()) {
      for (int i = 0; i < K; i++) {
        cout << "Cluster " << clusters[i].clusterId << " centroid : ";
        for (int j = 0; j < dimensions; j++) {
          cout << clusters[i].centroidComponent(j) << " ";    // Output to console
          outfile << clusters[i].centroidComponent(j) << " "; // Output to file
        }
        cout << endl;
        outfile << endl;
      }
      outfile.close();
    }
    else 
      cout << "Error: Unable to write to clusters.txt";
  }
};

class brf
{
private:
    /* data */
public:
    brf(/* args */);
    ~brf();
};

brf::brf(/* args */)
{
}

brf::~brf()
{
}
