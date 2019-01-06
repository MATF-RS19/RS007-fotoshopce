#pragma once


#include "image.h"
#include "abstract_operation.h"


namespace fs::ops
{

	/*
	* @brief Class that implements mirror operation.
	*/
	class MirrorOperation : public AbstractOperation
	{
		public:
			MirrorOperation(const Image &img);
			void apply(Image &img) override;
	};


	/*
	* @brief Class that implements rotations.
	*/
	class RotateLeftOperation : public AbstractOperation
	{
		public:
			RotateLeftOperation(const Image &img);
			void apply(Image &img) override;
	};


	/*
	* @brief Class that implements rotations.
	*/
	class RotateRightOperation : public AbstractOperation
	{
		public:
			RotateRightOperation(const Image &img);
			void apply(Image &img) override;
	};


	/*
	* @brief Class that implements resizing.
	*/
	class ResizeOperation : public AbstractOperation
	{
		public:
			explicit ResizeOperation(const Image &img, int width, int height);
			void apply(Image &img) override;
		private:
			int m_width, m_height;
	};

} // namespace fs::ops
