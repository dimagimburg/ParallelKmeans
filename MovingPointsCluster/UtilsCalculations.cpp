#include <math.h>
#include <stdio.h>
#include "UtilsCalculations.h"
#include "Cluster.h"

int Calculations_parallel_get_number_of_increments(Config* config, int my_id, int number_of_procs){
	int total_increments = (config->T / config->delta_t) + 1; // 0 up to 10 = 11 increments
	int number_of_increments_per_proccess = total_increments / number_of_procs;
	int remainder = total_increments % number_of_procs;
	if (my_id < remainder)
		return number_of_increments_per_proccess + 1;
	return number_of_increments_per_proccess;
}

double Calculations_parallel_get_start_offset_time(Config* config, int my_id, int number_of_procs){
	double offset = 0.0;
	for (int i = 0; i < my_id; i++){
		offset += config->delta_t * Calculations_parallel_get_number_of_increments(config, i, number_of_procs);
	}
	return offset;
}

void Calculations_get_cluster_id_min_distance(Config* config, ClusterInfo** master_all_cluster_infos, int num_procs, int* min_id, double* min_distance, double* min_delta_t){	
	*min_delta_t = -1;
	*min_distance = INFINITY;
	for (int i = 0; i < num_procs; i++){
		int number_of_incs_per_proc = Calculations_parallel_get_number_of_increments(config, i, num_procs);
		int number_of_infos_per_proc = number_of_incs_per_proc * config->number_of_clusters;
		for (int j = 0; j < number_of_infos_per_proc; j += config->number_of_clusters){
			// new delta_t
			double delta_t_current = master_all_cluster_infos[i][j].delta_t;
			double min_current = INFINITY;

			// find minimum inside a delta_t
			for (int clust = j; clust < j + config->number_of_clusters; clust++){
				for (int clust_to_cmp = clust + 1; clust_to_cmp < j + config->number_of_clusters; clust_to_cmp++){
					if (Cluster_get_distance_between_cluster_centers(&master_all_cluster_infos[i][clust], &master_all_cluster_infos[i][clust_to_cmp]) < min_current){
						min_current = Cluster_get_distance_between_cluster_centers(&master_all_cluster_infos[i][clust], &master_all_cluster_infos[i][clust_to_cmp]);
					}
				}
			}

			// check if minimum
			if (min_current < *min_distance){
				*min_distance = min_current;
				*min_delta_t = delta_t_current;
			}
			
			Cluster_print_info(&master_all_cluster_infos[i][j]);
		}
	}
}