#pragma once

#include <mpi.h>
#include <sstream>
#include "UtilsMPI.h"
#include "MovingPoint.h"
#include "Config.h"

void MPI_Custom_initialize_mpi(int* argc, char*** argv, int* my_id, int* numprocs, char* processor_name, int* namelen)
{
	MPI_Init(argc, argv);
	MPI_Comm_rank(MPI_COMM_WORLD, my_id);
	MPI_Comm_size(MPI_COMM_WORLD, numprocs);
	MPI_Get_processor_name(processor_name, namelen);
}

void MPI_Custom_create_moving_point_datatype(MPI_Datatype* MPI_CUSTOM_DATATYPE)
{
	/**
	Creates a new custom datatype for the moving points
	@param [MPI_Datatype* MPI_CUSTOM_DATATYPE] - reference for the MPI_Datatype object
	@return [void]
	*/

	// based on answer (http://stackoverflow.com/a/20709889/2698072)

	int	blocks[MOVING_POINT_NUMBER_OF_BLOCKS] = { 1, 1, 1, 1 };

	MPI_Datatype types[MOVING_POINT_NUMBER_OF_BLOCKS] = {    /* moving point internal types */
		MPI_INT,
		MPI_DOUBLE,
		MPI_DOUBLE,
		MPI_DOUBLE
	};

	MPI_Aint dis[MOVING_POINT_NUMBER_OF_BLOCKS] = {          /* internal displacements */
		offsetof(MovingPoint, index),
		offsetof(MovingPoint, a),
		offsetof(MovingPoint, b),
		offsetof(MovingPoint, radius)
	};

	MPI_Type_create_struct(MOVING_POINT_NUMBER_OF_BLOCKS, blocks, dis, types, MPI_CUSTOM_DATATYPE);
	MPI_Type_commit(MPI_CUSTOM_DATATYPE);
}

void MPI_Custom_create_config_datatype(MPI_Datatype* MPI_CUSTOM_DATATYPE)
{
	/**
	Creates a new custom datatype for the config
	@param [MPI_Datatype* MPI_CUSTOM_DATATYPE] - reference for the MPI_Datatype object
	@return [void]
	*/

	// based on answer (http://stackoverflow.com/a/20709889/2698072)

	int	blocks[CONFIG_NUMBER_OF_BLOCKS] = { 1, 1, 1, 1, 1 };

	MPI_Datatype types[CONFIG_NUMBER_OF_BLOCKS] = {    /* config internal types */
		MPI_INT,
		MPI_INT,
		MPI_DOUBLE,
		MPI_DOUBLE,
		MPI_INT
	};

	MPI_Aint dis[CONFIG_NUMBER_OF_BLOCKS] = {          /* internal displacements */
		offsetof(Config, number_of_points),
		offsetof(Config, number_of_clusters),
		offsetof(Config, delta_t),
		offsetof(Config, T),
		offsetof(Config, limit)
	};

	MPI_Type_create_struct(CONFIG_NUMBER_OF_BLOCKS, blocks, dis, types, MPI_CUSTOM_DATATYPE);
	MPI_Type_commit(MPI_CUSTOM_DATATYPE);
}

void MPI_Custom_create_cluster_info_datatype(MPI_Datatype* MPI_CUSTOM_DATATYPE){
	/**
	Creates a new custom datatype for the cluster info
	@param [MPI_Datatype* MPI_CUSTOM_DATATYPE] - reference for the MPI_Datatype object
	@return [void]
	*/

	// based on answer (http://stackoverflow.com/a/20709889/2698072)

	int	blocks[CONFIG_NUMBER_OF_BLOCKS] = { 1, 1, 1, 1, 1 };

	MPI_Datatype types[CONFIG_NUMBER_OF_BLOCKS] = {    /* config internal types */
		MPI_INT,
		MPI_DOUBLE,
		MPI_INT,
		MPI_DOUBLE,
		MPI_DOUBLE
	};

	MPI_Aint dis[CONFIG_NUMBER_OF_BLOCKS] = {          /* internal displacements */
		offsetof(ClusterInfo, id),
		offsetof(ClusterInfo, delta_t),
		offsetof(ClusterInfo, number_of_points),
		offsetof(ClusterInfo, center_x),
		offsetof(ClusterInfo, center_y)
	};

	MPI_Type_create_struct(CONFIG_NUMBER_OF_BLOCKS, blocks, dis, types, MPI_CUSTOM_DATATYPE);
	MPI_Type_commit(MPI_CUSTOM_DATATYPE);
}

void MPI_Custom_broadcast_config(int my_id, Config* config, MPI_Datatype* MPI_CUSTOM_CONFIG_DATATYPE){
	MPI_Bcast(config, 1, *MPI_CUSTOM_CONFIG_DATATYPE, MASTER_ID, MPI_COMM_WORLD);
}

void MPI_Custom_broadcast_moving_points(int my_id, MovingPoint** moving_points, Config* config, MPI_Datatype* MPI_CUSTOM_CONFIG_DATATYPE){
	MPI_Bcast(*moving_points, (*config).number_of_points , *MPI_CUSTOM_CONFIG_DATATYPE, MASTER_ID, MPI_COMM_WORLD);
}