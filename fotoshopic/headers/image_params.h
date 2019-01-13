#pragma once


#include <array>
#include <unordered_map>

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
* @brief Enum class representing filters.
*/
enum class filters
{
	none,
	autumn,
	bone,
	jet,
	winter,
	rainbow,
	ocean,
	summer,
	spring,
	cool,
	hsv,
	pink,
	hot,
};


/*
* @brief Enum class representing image type.
*/
enum class image_type
{
	color,
	grayscale,
};

/*
* @brief Structure that represents image parameters.
*/
struct image_parameters
{
	filters filter{filters::none};
	int current_top{-1}, current_bottom{-1}, current_left{-1}, current_right{-1};
	std::pair<int, int> size{-1, -1};
	image_type img_type{image_type::color};
	qstring_map<int> adjustment_map{qstring_pair("Brightness", 50),
									qstring_pair("Contrast", 50),
								 	qstring_pair("Sharpen", 0),
									qstring_pair("Vignette", 0),
									qstring_pair("Blur", 0),
									qstring_pair("Hue", 50),
									qstring_pair("Saturation", 50),
									qstring_pair("Value", 50)};
};
