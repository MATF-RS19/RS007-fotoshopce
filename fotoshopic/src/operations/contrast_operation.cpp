#include <iostream>
#include <opencv2/opencv.hpp>

#include "headers/operations/contrast_operation.hpp"


namespace rs::operations
{

	/*
	* @brief Builds instance of contrast_operation.
	*/
	contrast_operation::contrast_operation(int value)
		:	abstract_color_operation(std::move(value))
	{}

	/*
	* @brief Call operator that applies operation.
	*/
	void contrast_operation::operator()(cv::Mat &img) const
	{
		double contrast{m_value / 50.0};
		cv::multiply(img, cv::Scalar(contrast, contrast, contrast), img);
	}

} // namespace rs::operations
