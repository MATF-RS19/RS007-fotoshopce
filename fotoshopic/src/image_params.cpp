#include "headers/image_params.h"


ImageParams::ImageParams()
	:	corners{image_corners::top_left, image_corners::top_right, image_corners::bottom_left, image_corners::bottom_right},
		adjustment_map{{QString::fromStdString("Contrast"), 50},
						 {QString::fromStdString("Brightness"), 50},
						 {QString::fromStdString("Sharpen"), 0},
						 {QString::fromStdString("Vignette"), 0},
						 {QString::fromStdString("Blur"), 0},
						 {QString::fromStdString("Saturation"), 50}}

{}

