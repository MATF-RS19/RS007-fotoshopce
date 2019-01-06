#pragma once


#include "image.h"


// Forward declaration
class Image;


namespace fs::ops
{

	/*
	* @brief Abstract base class for all image related operations.
	*/
	class AbstractOperation
	{
		// Public member functions
		public:
			AbstractOperation(const Image &img);
			virtual ~AbstractOperation() {}
			virtual void apply(Image &img) = 0;
			void invert(Image &img);
		private:
			Image m_img;
	};

} // namespace fs::ops
