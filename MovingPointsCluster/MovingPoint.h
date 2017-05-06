#pragma once

#define MOVING_POINT_NUMBER_OF_BLOCKS 4

typedef struct moving_point {
	int index;
	double a;
	double b;
	double radius;
} MovingPoint;

void MovingPoint_print(MovingPoint* moving_point);
void MovingPoint_print(MovingPoint* moving_point, double delta_t, double T);
double MovingPoint_get_x(MovingPoint* moving_point, double delta_t, double T);
double MovingPoint_get_y(MovingPoint* moving_point, double delta_t, double T);