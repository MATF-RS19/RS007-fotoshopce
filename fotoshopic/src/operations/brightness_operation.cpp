#include <iostream>
#include <opencv2/opencv.hpp>

#include "headers/operations/brightness_operation.hpp"


namespace rs::operations
{

	/*
	* @brief Builds instance of brightness_operation.
	*/
	brightness_operation::brightness_operation(int value)
		:	abstract_color_operation(std::move(value))
	{}

	/*
	* @brief Call operator that applies operation.
	*/
	void brightness_operation::operator()(cv::Mat &img) const
	{
		double brightness{m_value - 50.0};
		cv::add(img, cv::Scalar(brightness, brightness, brightness), img);
	}

} // namespace rs::operations
