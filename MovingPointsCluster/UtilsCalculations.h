#pragma once
#include "Config.h"
#include "Cluster.h"

int Calculations_parallel_get_number_of_increments(Config* config, int my_id, int number_of_procs);
double Calculations_parallel_get_start_offset_time(Config* config, int my_id, int number_of_procs);
void Calculations_get_cluster_id_min_distance(Config* config, ClusterInfo** master_all_cluster_infos, int num_procs, int* min_id, double* min_distance, double* min_delta_t);