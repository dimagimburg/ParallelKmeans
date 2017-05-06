#pragma once

#define CONFIG_NUMBER_OF_BLOCKS 5

typedef struct config {
	int number_of_points;
	int number_of_clusters;
	double delta_t;
	double T;
	int limit;
} Config;

void Config_print(Config* config);