#include "Cluster.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void Cluster_print(Cluster* cluster){
	Cluster_print_info(cluster);
	for (int i = 0; i < cluster->number_of_points; i++){
		MovingPoint_print(cluster->moving_points[i]);
	}
}

void Cluster_print(Cluster* cluster, double delta_t, double T){
	Cluster_print_info(cluster);
	for (int i = 0; i < cluster->number_of_points; i++){
		MovingPoint_print(cluster->moving_points[i], delta_t, T);
	}
}

void Cluster_print_info(Cluster* cluster){
	printf("[Cluster] id: %d, delta_t: %.2f, number of moving points: %d, center x: %.6f, center y: %.6f\n", (*cluster).id, (*cluster).delta_t, (*cluster).number_of_points, (*cluster).center_x, (*cluster).center_y);
}

void Cluster_print_info(ClusterInfo* cluster){
	printf("[Cluster] id: %d, delta_t: %.2f, number of moving points: %d, center x: %.6f, center y: %.6f\n", (*cluster).id, (*cluster).delta_t, (*cluster).number_of_points, (*cluster).center_x, (*cluster).center_y);
}

void Cluster_init(Cluster* cluster, int id, double delta_t){
	cluster->id = id;
	cluster->delta_t = delta_t;
	cluster->center_x = 0;
	cluster->center_y = 0;
	cluster->number_of_points = 0;
	cluster->moving_points = (MovingPoint**)malloc(sizeof(MovingPoint*));
};

void Cluster_init_points(Cluster* cluster){
	cluster->number_of_points = 0;
}

void Cluster_add_moving_point(Cluster* cluster, MovingPoint* moving_point){
	if (cluster->number_of_points == 0){
		(*(cluster->moving_points)) = (MovingPoint*) malloc(sizeof(MovingPoint));
		(cluster->moving_points)[0] = moving_point;
	}
	else {
		MovingPoint** new_points = (MovingPoint**)realloc(cluster->moving_points, (cluster->number_of_points + 1)*sizeof(MovingPoint*));
		if (new_points == NULL){
			printf("cannot do realloc.\n\n");
			exit(1);
		}
		cluster->moving_points = new_points;
		(cluster->moving_points)[cluster->number_of_points] = moving_point;
	}
	cluster->number_of_points += 1;
}

void Cluster_clear_moving_points(Cluster* cluster){
	cluster->number_of_points = 0;
}

void Cluster_recenter(Cluster* cluster, double delta_t, double T){
	double sum_x = 0, sum_y = 0;
	for (int i = 0; i < cluster->number_of_points; i++){
		sum_x += MovingPoint_get_x(cluster->moving_points[i], delta_t, T);
		sum_y += MovingPoint_get_y(cluster->moving_points[i], delta_t, T);
	}
	cluster->center_x = sum_x / cluster->number_of_points;
	cluster->center_y = sum_y / cluster->number_of_points;
}

double Cluster_get_distance_from_center(Cluster* cluster, MovingPoint* moving_point, double delta_t, double T){
	double cluster_x, cluster_y, moving_point_x, moving_point_y;
	cluster_x = cluster->center_x;
	cluster_y = cluster->center_y;
	moving_point_x = MovingPoint_get_x(moving_point, delta_t, T);
	moving_point_y = MovingPoint_get_y(moving_point, delta_t, T);
	return sqrt(pow(moving_point_x - cluster_x, 2) + pow(moving_point_y - cluster_y, 2));
}

void Cluster_get_info(Cluster* cluster, ClusterInfo* cluster_info){
	cluster_info->center_x = cluster->center_x;
	cluster_info->center_y = cluster->center_y;
	cluster_info->delta_t = cluster->delta_t;
	cluster_info->id = cluster->id;
	cluster_info->number_of_points = cluster->number_of_points;
}

double Cluster_get_distance_between_cluster_centers(ClusterInfo* cl1, ClusterInfo* cl2){
	return sqrt(pow((cl1->center_x - cl2->center_x), 2) + pow((cl1->center_y - cl2->center_y), 2));
}