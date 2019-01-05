#pragma once


#include "edit_operations.h"


namespace fs::ops
{

	/*
	* @brief Enum class representing color.
	*/
	enum class colors
	{
		red,
		green,
		blue,
	};


	/*
	* @brief Class representing color operations.
	*/
	class ColorEditOperation : public BasicEditOperation
	{
		public:
			explicit ColorEditOperation(int from, int to, colors color);
			virtual void apply(Image &img) override;
			virtual void invert(Image &img) override;
		private:
			colors m_color;
	};

} // namespace fs::ops
