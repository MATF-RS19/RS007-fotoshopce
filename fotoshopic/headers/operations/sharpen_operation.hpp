#pragma once


#include "abstract_color_operation.hpp"


namespace rs::operations
{

	/*
	* @brief Class implementing sharpen operation.
	*/
	class sharpen_operation : public abstract_color_operation
	{
		/* Public member functions */
		public:
			explicit sharpen_operation(int value);
			void operator()(cv::Mat &img) const override;
	};

} // namespace rs::operations
