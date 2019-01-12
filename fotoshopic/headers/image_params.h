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
struct ImageParams
{
	std::array<image_corners, 4> corners{image_corners::top_left, image_corners::top_right, image_corners::bottom_left, image_corners::bottom_right};
	filters filter{filters::none};
	// Used for croping if image is zoomed
	int m_current_top{-1};
	int m_current_bottom{-1};
	int m_current_left{-1};
	int m_current_right{-1};
	std::pair<int, int> size{-1, -1};
	image_type img_type{image_type::color};
	// TODO: Remove hardcoded values.
	qstring_map<int> adjustment_map{qstring_pair("Contrast", 50),
									qstring_pair("Brightness", 50),
									qstring_pair("Sharpen", 0),
									qstring_pair("Vignette", 0),
									qstring_pair("Blur", 0),
									qstring_pair("Saturation", 50),
									qstring_pair("Hue", 50),
									qstring_pair("Value", 50)};
};
