#include "Config.h"
#include <stdio.h>

void Config_print(Config* config){
	printf("Config: number_of_points: %d, number_of_clusters: %d, delta_t: %.2f, T: %.2f, limit: %d\n", (*config).number_of_points, (*config).number_of_clusters, (*config).delta_t, (*config).T, (*config).limit);
}