#pragma once


#include <iostream>


// Forward declaration
class Image;


/*
* @brief Abstract base class for all image related operations.
*/
class AbstractOperation
{
	// Public member functions
	public:
		virtual Image apply(Image img) const = 0; 
		virtual Image invert(Image img) const = 0;
};


