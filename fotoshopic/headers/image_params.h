#pragma once

#include <unordered_map>

#include <QString>

#include "utils.h"

/*
* @brief Structure that represents image parameters.
*/
struct image_params
{
	image_params();

	int topleft_corner;
	int topright_corner;
	int bottomleft_corner;
	int bottomright_corner;
//	double m_brightness;
//	double m_contrast;
//	double m_sharpen;
//	double m_blur;
	qstring_map<std::pair<int, int>> m_adjustment_map;
};

