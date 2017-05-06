#pragma once

#include "Config.h"
#include "MovingPoint.h"

void File_loader_load_config(Config* config, char* file_path);
void File_loader_load_moving_points(MovingPoint** moving_points, Config* config, char* file_path);