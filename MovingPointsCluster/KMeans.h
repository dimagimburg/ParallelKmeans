#pragma once
#include "Config.h"
#include "Cluster.h"
#include "MovingPoint.h"

void KMeans_get_clusters(Config* config, Cluster** clusters, MovingPoint** moving_points, double current_delta_t);
void KMeans_initialize_clusters(Config* config, Cluster** clusters, MovingPoint** moving_points, double current_delta_t);
void KMeans_perform(Config* config, Cluster** clusters, MovingPoint** moving_points, double current_delta_t);
void KMeans_parallel_perform(Config* config, Cluster** clusters, MovingPoint** moving_points, double current_delta_t);