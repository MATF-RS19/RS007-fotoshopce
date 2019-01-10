#pragma once


#include <opencv2/opencv.hpp>

#include "abstract_color_operation.hpp"


namespace rs::operations
{

	/*
	* @brief Class implementing blur operation.
	*/
	class blur_operation : public abstract_color_operation
	{
		/* Public member functions */
		public:
			explicit blur_operation(int value);
			void operator()(cv::Mat &img) const override;
	};

} // namespace rs::operations
