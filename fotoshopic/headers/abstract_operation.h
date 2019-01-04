#pragma once


#include <iostream>


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
			virtual ~AbstractOperation() {}
			virtual void apply(Image &img) = 0;
			virtual void invert(Image &img) = 0;
	};

} // namespace fs::ops
