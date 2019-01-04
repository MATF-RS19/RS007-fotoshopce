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
			void apply(Image &img) override;
			void invert(Image &img) override;
	};


	/*
	* @brief Class that implements rotations.
	*/
	class RotateLeftOperation : public AbstractOperation
	{
		public:
			void apply(Image &img) override;
			void invert(Image &img) override;
	};


	/*
	* @brief Class that implements rotations.
	*/
	class RotateRightOperation : public AbstractOperation
	{
		public:
			void apply(Image &img) override;
			void invert(Image &img) override;
	};


	/*
	* @brief Class that implements resizing.
	*/
	class ResizeOperation : public AbstractOperation
	{
		public:
			explicit ResizeOperation(int width, int height);
			void apply(Image &img) override;
			void invert(Image &img) override;
		private:
			int m_width, m_height;
            Image m_old;
	};

} // namespace fs::ops
