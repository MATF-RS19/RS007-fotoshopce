#pragma once

#include <array>
#include <unordered_map>

#include <QString>

#include "utils.h"

/*
* @brief Enum class representing image corners.
*/
enum class image_corners
{
	top_left,
	top_right,
	bottom_left,
	bottom_right,
};


/*
* @brief Structure that represents image parameters.
*/
struct ImageParams
{
	ImageParams();

	std::array<image_corners, 4> corners;

	qstring_map<int> adjustment_map;
};

