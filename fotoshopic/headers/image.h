#pragma once


#include <iostream>
#include <vector>

#include <QFile>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include "headers/image_params.h"
#include "headers/utils.h"


/* Forward declarations */
class MainWindow;


/*
* @brief Enum class representing image type.
*/
enum class image_type
{
	color,
	grayscale,
};


/*
* @brief Class representing an OpenCV image with utility parameters.
*/
class Image
{
	// Friend declarations
	friend MainWindow;

	// Public member functions
	public:
		explicit Image(const cv::Mat& img = cv::Mat(), std::string filename = "");
		~Image();

	// Private member functions
	private:
		cv::Mat get_current();

	// Private member variables
	private:
		cv::Mat m_img;
		std::string m_filename;
		image_type m_type;
		std::vector<ImageParams> m_param_list;
		unsigned long m_index;
};
