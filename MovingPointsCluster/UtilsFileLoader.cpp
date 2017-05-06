#pragma warning(disable:4996)

#include "UtilsFileLoader.h"
#include <fstream>


void File_loader_load_config(Config* config, char* file_path)
{
	int number_of_points, number_of_clusters, limit;
	double delta_t, T;

	FILE* f;
	errno_t errorCode = fopen_s(&f, file_path, "r");
	if (errorCode != 0)
	{
		printf("Error opening file!\n");
		exit(1);
	}
	int row = fscanf_s(f, "%d %d %lf %lf %d\n", &number_of_points, &number_of_clusters, &delta_t, &T, &limit);
	fclose(f);

	config->number_of_points = number_of_points;
	config->number_of_clusters = number_of_clusters;
	config->delta_t = delta_t;
	config->T = T;
	config->limit = limit;
}

void File_loader_load_moving_points(MovingPoint** moving_points, Config* config, char* file_path)
{
	FILE* f;
	int index;
	double a, b, radius;

	// allocate memory for all moving points
	(*moving_points) = (MovingPoint*)malloc((*config).number_of_points * sizeof(MovingPoint));

	// load moving points from file
	f = fopen(file_path, "r");

	// first line is config line - ignore (http://stackoverflow.com/a/16108311/2698072)
	int row = fscanf_s(f, "%*[^\n]\n", NULL);

	while (row != EOF) {
		MovingPoint moving_point;
		row = fscanf_s(f, "%d %lf %lf %lf\n", &index, &a, &b, &radius);
		moving_point.index = index;
		moving_point.a = a;
		moving_point.b = b;
		moving_point.radius = radius;
		(*moving_points)[index] = moving_point;
	}

	fclose(f);

}