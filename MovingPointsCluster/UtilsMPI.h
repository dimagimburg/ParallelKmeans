#pragma once
#include "Config.h"
#include "MovingPoint.h"
#include "Cluster.h"

#define MASTER_ID 0
#define TAG 0

void MPI_Custom_initialize_mpi(int* argc, char*** argv, int* myid, int* numprocs, char* processor_name, int* namelen);
void MPI_Custom_create_moving_point_datatype(MPI_Datatype* MPI_CUSTOM_DATATYPE);
void MPI_Custom_create_config_datatype(MPI_Datatype* MPI_CUSTOM_DATATYPE);
void MPI_Custom_create_cluster_info_datatype(MPI_Datatype* MPI_CUSTOM_DATATYPE);
void MPI_Custom_broadcast_config(int my_id, Config* config, MPI_Datatype* MPI_CUSTOM_CONFIG_DATATYPE);
void MPI_Custom_broadcast_moving_points(int my_id, MovingPoint** moving_points, Config* config, MPI_Datatype* MPI_CUSTOM_CONFIG_DATATYPE);