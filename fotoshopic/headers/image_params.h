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
	// TODO: remove this, use the corners array instead. [@milanilic332]
	int topleft_corner = 0;
	int topright_corner = 1;
	int bottomleft_corner = 2;
	int bottomright_corner = 3;
	qstring_map<int> adjustment_map;
};

