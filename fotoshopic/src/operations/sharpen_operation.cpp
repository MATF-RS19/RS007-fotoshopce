#include <iostream>
#include <opencv2/opencv.hpp>

#include "headers/operations/sharpen_operation.hpp"


namespace rs::operations
{

	/*
	* @brief Builds instance of sharpen_operation.
	*/
	sharpen_operation::sharpen_operation(int value)
		:	abstract_color_operation(std::move(value))
	{}

	/*
	* @brief Call operator that applies operation.
	*/
	void sharpen_operation::operator()(cv::Mat &img) const
	{
		if(m_value) {
			cv::Mat gaussian;
			cv::GaussianBlur(img, gaussian, cv::Size(11, 11), m_value);
			cv::addWeighted(img, 1.5, gaussian, -0.5, 0, img);
		}
	}

} // namespace rs::operations
