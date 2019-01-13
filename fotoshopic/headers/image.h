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


/* Forward declarations */
class MainWindow;

/*
* @brief Class representing an OpenCV image with utility parameters.
*/
class image
{
	/* Friend declarations */
	friend MainWindow;

	/* Public member functions */
	public:
		explicit image(const cv::Mat& img = cv::Mat());
		image(const image &img);
		~image();

		static image set_parameters(image img, image_parameters params);

	/* Private member functions */
		static void apply_filter(cv::Mat &img, int filter);

	/* Private member variables */
	private:
		cv::Mat m_img;
};
