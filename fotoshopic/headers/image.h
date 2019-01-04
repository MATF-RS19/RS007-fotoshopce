#pragma once


#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>


/*
* @brief TODO: docstring [@milanilic332]
*/
class Image
{
	public:
		Image(const cv::Mat& img, const std::string& filename);
		Image();
		Image(const Image& im);
		Image(Image&& im);
		~Image();
		Image operator=(const Image& im);
		Image operator=(Image&& im);
		//    void setType(int type);
		//    void setCurrentZoom(double zoom);
		//    int getType() const;
		//    double getCurrentZoom() const;

		// Defining image
		cv::Mat mImg;
		std::string mFileName;

		// Image params
		int mType = 1;
};
