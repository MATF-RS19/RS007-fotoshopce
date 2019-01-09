#include "headers/image_params.h"


image_params::image_params()
	:	topleft_corner(0),
		topright_corner(1),
		bottomleft_corner(2),
		bottomright_corner(3),
		m_adjustment_map{ { QString::fromStdString("Contrast"), {1, 1} },
						  { QString::fromStdString("Brightness"), {0, 0} },
						  { QString::fromStdString("Sharpen"), {0, 0} },
						  { QString::fromStdString("Vignette"), {0, 0} },
						  { QString::fromStdString("Blur"), {0, 0} },
						  { QString::fromStdString("Saturation"), {0, 0} } }

{}

