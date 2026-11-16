#include "lab1_omp.h"
#include <bits/stdc++.h>
#include <malloc.h>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
using namespace std;
void kmeans_sequential(int N, int K, int* data_points, int** data_point_cluster, float** centroids, int* num_iterations);

vector<int> vect;
int num_threads;

struct Point{
    double x; //x coordinate
    double y; //y coordinate
    double z; //z coordinate
    int cluster; //cluster to which the point belongs
};

//helper function prototypes
void mean_recompute(int K, int N, Point points[],Point centr[]);
Point addtwo(Point a, Point b);
double euclid(Point a, Point b);
Point mean(Point arr[], int N);
void assignclusters(Point points[], Point centr[], int K, int N);
void putback(Point centr[],int K);
//Point points[N];


//driver function
void kmeans_omp(int num_threads,int N, int K, int* data_points, int** data_point_cluster, float** centroids, int* num_iterations){
    Point points[N];
    ::num_threads=num_threads;

    //---------------------------
    int j=0;
    for (int i=0;  i<(3*N); i+=3){
        points[j].x  = data_points[i];
        points[j].y  = data_points[i+1];
        points[j].z  = data_points[i+2];
        j++;
    }

    
    //---------------------------

    // K-Means++ centroid initialization
    Point centr[K];

    // Use the same fixed seed as Sequential
    mt19937 rng(10);

    // Step 1: Randomly select the first centroid
    uniform_int_distribution<int> first_dist(0, N - 1);
    int first_index = first_dist(rng);

    centr[0] = points[first_index];

    // Step 2: Select the remaining K-1 centroids
    for (int c = 1; c < K; c++) {

        vector<double> min_distances(N);
        double total_distance = 0.0;

        // Calculate squared distance to the nearest selected centroid
        for (int i = 0; i < N; i++) {

            double min_distance = numeric_limits<double>::max();

            for (int j = 0; j < c; j++) {

                double dx = points[i].x - centr[j].x;
                double dy = points[i].y - centr[j].y;
                double dz = points[i].z - centr[j].z;

                double distance = dx * dx + dy * dy + dz * dz;

                if (distance < min_distance) {
                    min_distance = distance;
                }
            }

            min_distances[i] = min_distance;
            total_distance += min_distance;
        }

        // Step 3: Select the next centroid using weighted probability
        int selected_index = 0;

        if (total_distance > 0.0) {

            uniform_real_distribution<double> probability(0.0, total_distance);
            double target = probability(rng);

            double cumulative_distance = 0.0;

            for (int i = 0; i < N; i++) {

                cumulative_distance += min_distances[i];

                if (target < cumulative_distance) {
                    selected_index = i;
                    break;
                }
            }
        }
        else {
            // Fallback when all points have zero distance
            uniform_int_distribution<int> random_point(0, N - 1);
            selected_index = random_point(rng);
        }

        centr[c] = points[selected_index];
    }

    //---------------------------

    //array to keep a track of distances of a point from all centroids, to take the minimum out of them
    double distances[K];
    //computing distance of a point and assigning all
    for (int i=0; i<N; i++){
        for(int j=0; j<K ; j++){
            distances[j] = euclid(points[i], centr[j]);
        }
        int index = 0;
        for(int i = 1; i < K; i++){
            if(distances[i] < distances[index])
                index = i;
        }
        points[i].cluster = index;
    }

    //---------------------------

    mean_recompute(K, N, points,centr);
    putback(centr, K);

    //---------------------------

    int iterations = 1;
    int count;
    do {
        mean_recompute(K, N, points,centr);
        putback(centr, K);
        //storing old values for convergence check
        int old[N];
        for (int i=0; i<N; i++){
            old[i] = points[i].cluster;
        }
        assignclusters(points, centr, K, N);
        iterations++;
        count = 0;
        for (int i=0; i<N; i++){
            if (old[i] == points[i].cluster)
                count++;
        }
    } while(count!=N);

    //---------------------------

    *data_point_cluster= (int*) calloc(4*N, sizeof(int));
    *centroids = (float*) calloc(vect.size(), sizeof(float));

    int q = 0;
    for (int i = 0; i< 4*N; i+=4){
        data_point_cluster[0][i] = points[q].x;
        data_point_cluster[0][i+1] = points[q].y;
        data_point_cluster[0][i+2] = points[q].z;
        data_point_cluster[0][i+3] = points[q].cluster;
        q++;
    }
    for (int i = 0; i<vect.size(); i++){
        centroids[0][i] = vect[i];
    }
    *num_iterations = vect.size() / (K * 3) - 1;
}

//funtion to recompute the new centroids for each cluster
//N is the total number of data points and K is the total number of clusters
void mean_recompute(int K, int N, Point points[], Point centr[]) {

    int count[K];
    Point sum[K];

    // Initialise counters and coordinate sums
    for (int i = 0; i < K; i++) {
        count[i] = 0;
        sum[i].x = 0;
        sum[i].y = 0;
        sum[i].z = 0;
        sum[i].cluster = 0;
    }

    // Calculate the sum of coordinates for each cluster
    for (int i = 0; i < N; i++) {
        int cluster_id = points[i].cluster;

        count[cluster_id]++;

        sum[cluster_id].x += points[i].x;
        sum[cluster_id].y += points[i].y;
        sum[cluster_id].z += points[i].z;
    }

    // Calculate new centroids
    for (int i = 0; i < K; i++) {
        if (count[i] > 0) {
            centr[i].x = sum[i].x / count[i];
            centr[i].y = sum[i].y / count[i];
            centr[i].z = sum[i].z / count[i];
        }
        // If a cluster is empty, retain its previous centroid
    }
}

//assuming they are the same cluster
Point addtwo(Point a, Point b){
    Point ans;
    ans.x = a.x + b.x;
    ans.y = a.y + b.y;
    ans.z = a.z + b.z;
    ans.cluster = a.cluster;
    return ans;
}

int checkClosestCluster(Point points[], Point centr[],int K, int N, int i){
  double distances[K];
  for(int j=0; j<K ; j++){
      distances[j] = euclid(points[i], centr[j]);
  }
  int index = 0;
  for(int i = 1; i < K; i++)
  {
      if(distances[i] < distances[index])
          index = i;
  }
  return index;
}


void assignclusters(Point points[], Point centr[],int K, int N){
    double distances[K];
    //computing distance of a point and assigning all the data points, a centroid/cluster value
    #pragma omp parallel num_threads(num_threads)
    {
    #pragma omp for
    for (int i=0; i<N; i++)
    {
        //assigning the minimum distance cluster, which is an index
        points[i].cluster = checkClosestCluster(points, centr,K, N, i);
        // points[i].cluster = index;
    }
  }
}
//function to calculate euclidea distance between two points
double euclid(Point a, Point b){
    double x = a.x- b.x;
    double y = a.y- b.y;
    double z = a.z- b.z;
    double dist = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    return dist;
}

void putback(Point centr[],int K){
    for (int i =0; i<K; i++) {
        vect.push_back(centr[i].x);
        vect.push_back(centr[i].y);
        vect.push_back(centr[i].z);
    }
}
