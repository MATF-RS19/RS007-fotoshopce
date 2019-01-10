#include <iostream>
#include <opencv2/opencv.hpp>

#include "headers/operations/blur_operation.hpp"


namespace rs::operations
{

	/*
	* @brief Builds instance of blur_operation.
	*/
	blur_operation::blur_operation(int value)
		:	abstract_color_operation(std::move(value))
	{}

	/*
	* @brief Call operator that applies operation.
	*/
	void blur_operation::operator()(cv::Mat &img) const
	{
		if(m_value) {
			cv::GaussianBlur(img, img, cv::Size(11, 11), m_value);
			cv::addWeighted(img, 2.5, img, -1.5, 0, img);
		}
	}

} // namespace rs::operations
