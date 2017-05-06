#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "KMeans.h"
#include "Cluster.h"

#define APPROXIMATION 0.00000001
#define PARALLEL 1 // par or seq

void KMeans_get_clusters(Config* config, Cluster** clusters, MovingPoint** moving_points, double current_delta_t){
	KMeans_initialize_clusters(config, clusters, moving_points, current_delta_t);
	if (PARALLEL == 1){
		KMeans_parallel_perform(config, clusters, moving_points, current_delta_t);
	}
	else {
		KMeans_perform(config, clusters, moving_points, current_delta_t);
	}
	
}

void KMeans_initialize_clusters(Config* config, Cluster** clusters, MovingPoint** moving_points, double current_delta_t){
	for (int i = 0; i < config->number_of_clusters; i++){
		Cluster clstr;
		Cluster_init(&clstr, i, current_delta_t);
		clstr.center_x = MovingPoint_get_x(&(*moving_points)[i], current_delta_t, config->T);
		clstr.center_y = MovingPoint_get_y(&(*moving_points)[i], current_delta_t, config->T);
		clusters[i] = (Cluster*)malloc(sizeof(Cluster));
		*(clusters[i]) = clstr;
	}
}

void KMeans_perform(Config* config, Cluster** clusters, MovingPoint** moving_points, double current_delta_t){
	// performs the KMEANS algorithm on all points based on config struct
	double* centers = (double*)malloc(config->number_of_clusters * 2 * sizeof(double));
	for (int i = 0; i < config->limit; i++){
		// main alg loop

		for (int j = 0; j < config->number_of_clusters; j++){
			// claculate centers for later check if clusters changed
			Cluster_init_points(clusters[j]);
			centers[2 * j] = clusters[j]->center_x;
			centers[2 * j + 1] = clusters[j]->center_y;
		}
		
		for (int p = 0; p < config->number_of_points; p++){
			// go along all points and attach each point to a closest cluster 

			Cluster* closest;
			double min_distance = Cluster_get_distance_from_center(&((*clusters)[0]), &(*moving_points)[p], current_delta_t, config->T);
			closest = clusters[0];

			for (int c = 1; c < config->number_of_clusters; c++){
				double new_distance = Cluster_get_distance_from_center(clusters[c], &(*moving_points)[p], current_delta_t, config->T);
				if (new_distance < min_distance){
					min_distance = new_distance;
					closest = clusters[c];
				}
			}
			Cluster_add_moving_point(closest, &(*moving_points)[p]);
		}

		for (int j = 0; j < config->number_of_clusters; j++){
			// calculate new centers for clusters
			Cluster_recenter(clusters[j], current_delta_t, config->T);
		}

		int not_changed_clusters = 0;
		for (int j = 0; j < config->number_of_clusters; j++){
			// check if cluster changed, if not return from function
			if (
				abs(centers[2 * j] - clusters[j]->center_x) < APPROXIMATION &&
				abs(centers[2 * j + 1] - clusters[j]->center_y) < APPROXIMATION){
				not_changed_clusters++;
			}
		}

		if (not_changed_clusters == config->number_of_clusters){
			free(centers);
			return;
		}
	}
	free(centers);
}

void KMeans_parallel_perform(Config* config, Cluster** clusters, MovingPoint** moving_points, double current_delta_t){
	int k, p;
	double* centers = (double*)malloc(config->number_of_clusters * 2 * sizeof(double));
	for (int i = 0; i < config->limit; i++){

		for (int j = 0; j < config->number_of_clusters; j++){
			Cluster_init_points(clusters[j]);
			centers[2 * j] = clusters[j]->center_x;
			centers[2 * j + 1] = clusters[j]->center_y;
		}

		for (p = 0; p < config->number_of_points; p++){
			Cluster* closest;
			double min_distance = Cluster_get_distance_from_center(&((*clusters)[0]), &(*moving_points)[p], current_delta_t, config->T);
			closest = clusters[0];

			for (int c = 1; c < config->number_of_clusters; c++){
				double new_distance = Cluster_get_distance_from_center(clusters[c], &(*moving_points)[p], current_delta_t, config->T);
				if (new_distance < min_distance){
					min_distance = new_distance;
					closest = clusters[c];
				}
			}
			Cluster_add_moving_point(closest, &(*moving_points)[p]);
		}

		// parallelize with OMP
		#pragma omp parallel shared(config, clusters, current_delta_t) private(k)
		{
			#pragma omp for
			for (k = 0; k < config->number_of_clusters; k++){
				Cluster_recenter(clusters[k], current_delta_t, config->T);
			}
		}

		int not_changed_clusters = 0;
		for (int j = 0; j < config->number_of_clusters; j++){
			if (
				abs(centers[2 * j] - clusters[j]->center_x) < APPROXIMATION &&
				abs(centers[2 * j + 1] - clusters[j]->center_y) < APPROXIMATION){
				not_changed_clusters++;
			}
		}

		if (not_changed_clusters == config->number_of_clusters){
			free(centers);
			return;
		}
	}
	free(centers);
}