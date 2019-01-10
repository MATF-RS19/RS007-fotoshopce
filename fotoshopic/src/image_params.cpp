#include "headers/utils.h"
#include "headers/image_params.h"


ImageParams::ImageParams()
	:	corners{image_corners::top_left, image_corners::top_right, image_corners::bottom_left, image_corners::bottom_right},
		filter{filters::none},
		adjustment_map{qstring_pair("Contrast", 50),
					   qstring_pair("Brightness", 50),
					   qstring_pair("Sharpen", 0),
					   qstring_pair("Vignette", 0),
					   qstring_pair("Blur", 0),
					   qstring_pair("Saturation", 50)}
	{}

