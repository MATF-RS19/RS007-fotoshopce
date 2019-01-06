#include "../headers/abstract_operation.h"


namespace fs::ops
{

	/*
	* @brief Builds instance of AbstractOperation.
	*
	* @param img Image to store in operation.
	*/
	AbstractOperation::AbstractOperation(const Image &img)
		:	m_img(img)
	{}

	/*
	* @brief Inverts image operation.
	*/
	void AbstractOperation::invert(Image &img)
	{
		img = m_img;
	}

} // namespace fs::ops
