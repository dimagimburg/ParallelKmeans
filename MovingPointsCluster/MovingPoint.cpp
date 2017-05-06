#include <stdio.h>
#include <math.h>
#include "MovingPoint.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
void MovingPoint_print(MovingPoint* moving_point)
{
	printf("[MovingPoint] index: %d, a: %.6f, b: %.6f, radius: %.6f\n", (*moving_point).index, (*moving_point).a, (*moving_point).b, (*moving_point).radius);
}

void MovingPoint_print(MovingPoint* moving_point, double delta_t, double T){
	printf("[MovingPoint] index: %d, x: %.6f, y: %.6f, radius: %.6f\n", (*moving_point).index, MovingPoint_get_x(moving_point, delta_t, T), MovingPoint_get_y(moving_point, delta_t, T), (*moving_point).radius);
}

double MovingPoint_get_x(MovingPoint* moving_point, double delta_t, double T)
{
	return (*moving_point).a + ((*moving_point).radius * cos((2 * M_PI * delta_t) / T));
}

double MovingPoint_get_y(MovingPoint* moving_point, double delta_t, double T)
{
	return (*moving_point).b + ((*moving_point).radius * sin((2 * M_PI * delta_t) / T));
}