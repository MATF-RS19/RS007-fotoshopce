#pragma once


#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include "headers/image_params.h"
#include "headers/utils.h"

/*
* @brief TODO: docstring [@milanilic332]
*/
class Image
{
	public:
		Image(const cv::Mat& img, const std::string& filename);
		Image();
		~Image();

		// Defining image
		cv::Mat m_img;
		std::string m_filename;

		// Image params
		int m_type;

		std::vector<image_params> param_list;
		unsigned long index{0};
		cv::Mat get_current();
};
