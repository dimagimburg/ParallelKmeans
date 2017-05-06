#pragma once
#include "MovingPoint.h"

typedef struct cluster {
	int id;
	double delta_t;
	int number_of_points;
	double center_x;
	double center_y;
	MovingPoint** moving_points;
} Cluster;

typedef struct cluster_info {
	int id;
	double delta_t;
	int number_of_points;
	double center_x;
	double center_y;
} ClusterInfo;

void Cluster_print(Cluster* cluster);
void Cluster_print(Cluster* cluster, double delta_t, double T);
void Cluster_print_info(Cluster* cluster);
void Cluster_print_info(ClusterInfo* cluster);
void Cluster_init(Cluster* cluster, int id, double delta_t);
void Cluster_init_points(Cluster* cluster);
void Cluster_add_moving_point(Cluster* cluster, MovingPoint* moving_point);
void Cluster_clear_moving_points(Cluster* cluster);
void Cluster_recenter(Cluster* cluster, double delta_t, double T);
double Cluster_get_distance_from_center(Cluster* cluster, MovingPoint* moving_point, double delta_t, double T);
void Cluster_get_info(Cluster* cluster, ClusterInfo* cluster_info);
double Cluster_get_distance_between_cluster_centers(ClusterInfo* cl1, ClusterInfo* cl2);