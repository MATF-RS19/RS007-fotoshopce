#pragma once


#include <iostream>
#include <opencv2/opencv.hpp>


namespace rs::operations
{

	/*
	* @brief Abstract class for all color related operations.
	*/
	class abstract_color_operation
	{
		/* Public member functions */
		public:
			explicit abstract_color_operation(int value)
				:	m_value{std::move(value)}
			{}

			inline void set_value(int value) { m_value = std::move(value); }
			virtual void operator()(cv::Mat &img) const = 0;
		/* Protected member variables */
		protected:
			int m_value;
	};

} // namespace rs::operations

