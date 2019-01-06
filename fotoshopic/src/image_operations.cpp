#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "../headers/image_operations.h"
#include "../headers/image.h"


namespace fs::ops
{

	// Mirror flip image
	void MirrorOperation::apply(Image &img)
	{
		cv::flip(img.m_img, img.m_img, 1);
	}

	// Reverse image mirror flip
	void MirrorOperation::invert(Image &img)
	{
		apply(img);
	}

	// Rotate image left
	void RotateLeftOperation::apply(Image &img)
	{
		cv::rotate(img.m_img, img.m_img, cv::ROTATE_90_COUNTERCLOCKWISE);
	}

	// Invert left rotation
	void RotateLeftOperation::invert(Image &img)
	{
		cv::rotate(img.m_img, img.m_img, cv::ROTATE_90_CLOCKWISE);
	}

	// Rotate image right
	void RotateRightOperation::apply(Image &img)
	{
		cv::rotate(img.m_img, img.m_img, cv::ROTATE_90_CLOCKWISE);
	}

	// Invert right rotation
	void RotateRightOperation::invert(Image &img)
	{
		cv::rotate(img.m_img, img.m_img, cv::ROTATE_90_COUNTERCLOCKWISE);
	}

	// Constructor for resize operation
	ResizeOperation::ResizeOperation(int width, int height)
		:	m_width{std::move(width)},
			m_height{std::move(height)}
	{}

	// Resize image
	void ResizeOperation::apply(Image &img)
	{
		m_old = img;
		cv::resize(img.m_img, img.m_img, cv::Size(m_width, m_height));
	}

	// Reverse image resizing
	void ResizeOperation::invert(Image &img)
	{
		img = m_old;
	}

} // namespace fs:ops

