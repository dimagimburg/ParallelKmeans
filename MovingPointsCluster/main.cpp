#include <math.h>
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "UtilsMPI.h"
#include "UtilsFileLoader.h"
#include "UtilsCalculations.h"
#include "MovingPoint.h"
#include "Config.h"
#include "Cluster.h"
#include "KMeans.h"

#define INPUT_FILE_PATH "kmean10.txt"
//#define PARALLEL 1

// TODO:
// 1. check if sending moving points with chunks helps with timing
// 2. implement parallel with CUDA

int main(int argc, char *argv[])
{
	// ==================== step 1 - initialize variables and MPI ====================
	int				my_id, num_procs, namelen, number_of_increments;
	double			start_offset, time_start, time_end;
	char			processor_name[MPI_MAX_PROCESSOR_NAME];
	Config			config;
	MovingPoint*	moving_points;
	Cluster***		clusters_holders;
	ClusterInfo**	master_all_cluster_infos;
	MPI_Status		status;

	// init of custom datatype for [MovingPoint, Config, ClusterInfo] struct to send 
	MPI_Datatype	MPI_CUSTOM_MOVING_POINT;
	MPI_Datatype	MPI_CUSTOM_CONFIG;
	MPI_Datatype	MPI_CUSTOM_CLUSTER_INFO;

	MPI_Custom_initialize_mpi(&argc, &argv, &my_id, &num_procs, processor_name, &namelen);
	MPI_Custom_create_moving_point_datatype(&MPI_CUSTOM_MOVING_POINT);
	MPI_Custom_create_config_datatype(&MPI_CUSTOM_CONFIG);
	MPI_Custom_create_cluster_info_datatype(&MPI_CUSTOM_CLUSTER_INFO);

	// ================ step 2 - send and get all information needed =================
	if (my_id == MASTER_ID){
		time_start = MPI_Wtime();
		File_loader_load_config(&config, INPUT_FILE_PATH);
		File_loader_load_moving_points(&moving_points, &config, INPUT_FILE_PATH);
	}

	MPI_Custom_broadcast_config(my_id, &config, &MPI_CUSTOM_CONFIG);

	if (my_id != MASTER_ID)
		moving_points = (MovingPoint*)malloc(config.number_of_points * sizeof(MovingPoint));

	MPI_Custom_broadcast_moving_points(my_id, &moving_points, &config, &MPI_CUSTOM_MOVING_POINT);

	// =================== step 3 - start kmeans parallel algorithm ==================

	number_of_increments = Calculations_parallel_get_number_of_increments(&config, my_id, num_procs);
	start_offset = Calculations_parallel_get_start_offset_time(&config, my_id, num_procs);
	printf("my id: %d, number of increments to do: %d, start offset: %.2f\n", my_id, number_of_increments, start_offset);
	clusters_holders = (Cluster***)malloc(number_of_increments * sizeof(Cluster**));
	for (int i = 0; i < number_of_increments; i++){
		printf("my id: %d, offset: %.3f\n", my_id, start_offset + config.delta_t * i);
		clusters_holders[i] = (Cluster**)malloc(config.number_of_clusters * sizeof(Cluster));
		KMeans_get_clusters(&config, clusters_holders[i], &moving_points, start_offset + config.delta_t * i);
	}

	// ========================== step 4 - get/send results ==========================
	ClusterInfo* clusters_info_final = (ClusterInfo*)malloc(number_of_increments * config.number_of_clusters * sizeof(ClusterInfo));

	for (int i = 0; i < number_of_increments; i++){
		for (int j = 0; j < config.number_of_clusters; j++){
			ClusterInfo ci;
			Cluster_get_info(clusters_holders[i][j], &ci);
			clusters_info_final[i*config.number_of_clusters + j] = ci;
		}
	}
	
	if (my_id == MASTER_ID){
		master_all_cluster_infos = (ClusterInfo**)malloc(num_procs * sizeof(ClusterInfo*));
		for (int i = 0; i < num_procs; i++){
			int number_of_incs_per_proc = Calculations_parallel_get_number_of_increments(&config, i, num_procs);
			int number_of_infos_per_proc = number_of_incs_per_proc * config.number_of_clusters;
			master_all_cluster_infos[i] = (ClusterInfo*)malloc(number_of_infos_per_proc * sizeof(ClusterInfo));
		}

		master_all_cluster_infos[0] = clusters_info_final;
		
		for (int i = 1; i < num_procs; i++){
			int number_of_incs_per_proc = Calculations_parallel_get_number_of_increments(&config, i, num_procs);
			int number_of_infos_per_proc = number_of_incs_per_proc * config.number_of_clusters;
			MPI_Recv(master_all_cluster_infos[i], number_of_infos_per_proc, MPI_CUSTOM_CLUSTER_INFO, i, TAG, MPI_COMM_WORLD, &status);
		}
		
	}
	else {
		MPI_Send(clusters_info_final, number_of_increments * config.number_of_clusters, MPI_CUSTOM_CLUSTER_INFO, MASTER_ID, TAG, MPI_COMM_WORLD);
	}

	// ========================= step 4 - data agrregation =========================
	if (my_id == MASTER_ID){
		int min_id;
		double min_distance, min_delta_t;
		Calculations_get_cluster_id_min_distance(&config, master_all_cluster_infos, num_procs, &min_id, &min_distance, &min_delta_t);
		time_end = MPI_Wtime();

		printf("\n\nMin dist = %.6f, \nMin delta_t = %.2f\n", min_distance, min_delta_t);
		printf("\nTime to calculate: %.6f\n", abs(time_start - time_end));

		FILE * out = fopen("output.txt", "w");
		fprintf(out, "d = %.2f\n", min_distance);
		fprintf(out, "t = %.2f\n", min_delta_t);
		fprintf(out, "centers of the clusters\n");

		for (int i = 0; i < num_procs; i++){
			int number_of_incs_per_proc = Calculations_parallel_get_number_of_increments(&config, i, num_procs);
			int number_of_infos_per_proc = number_of_incs_per_proc * config.number_of_clusters;
			for (int j = 0; j < number_of_infos_per_proc; j++){
				if (master_all_cluster_infos[i][j].delta_t == min_delta_t){
					fprintf(out, "x = %.6f, y = %.6f\n", master_all_cluster_infos[i][j].center_x, master_all_cluster_infos[i][j].center_y);
				}
			}
		}

		fclose(out);
	}

	

	// finalize, free memory, close files
	// TODO: free clusters
	free(moving_points);
	//for (int i = 0; i < number_of_increments; i++){
	//	free(clusters_holders[i]);
	//}
	//free(clusters_holders);
	MPI_Type_free(&MPI_CUSTOM_MOVING_POINT);
	MPI_Finalize();

	return 0;
}