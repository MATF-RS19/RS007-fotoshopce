#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "../headers/image_operations.h"
#include "../headers/image.h"


namespace fs::ops
{

	/*
	* @brief Builds instance of MirrorOperation.
	*/
	MirrorOperation::MirrorOperation(const Image &img)
		:	AbstractOperation(img)
	{}

	/*
	* @brief Mirror flip image.
	*/
	void MirrorOperation::apply(Image &img)
	{
		cv::flip(img.m_img, img.m_img, 1);
	}

	/*
	* @brief Builds instance of RotateLeftOperation.
	*/
	RotateLeftOperation::RotateLeftOperation(const Image &img)
		:	AbstractOperation(img)
	{}

	/*
	* @brief Rotate image left.
	*/
	void RotateLeftOperation::apply(Image &img)
	{
		cv::rotate(img.m_img, img.m_img, cv::ROTATE_90_COUNTERCLOCKWISE);
	}

	/*
	* @brief Builds instance of RotateRightOperation.
	*/
	RotateRightOperation::RotateRightOperation(const Image &img)
		:	AbstractOperation(img)
	{}

	/*
	* @brief Rotate image right.
	*/
	void RotateRightOperation::apply(Image &img)
	{
		cv::rotate(img.m_img, img.m_img, cv::ROTATE_90_CLOCKWISE);
	}

	/*
	* @brief Builds instance of ResizeOperation.
	*/
	ResizeOperation::ResizeOperation(const Image &img, int width, int height)
		:	AbstractOperation(img),
			m_width{std::move(width)},
			m_height{std::move(height)}
	{}

	/*
	* @brief Resize image.
	*/
	void ResizeOperation::apply(Image &img)
	{
		cv::resize(img.m_img, img.m_img, cv::Size(m_width, m_height));
	}

} // namespace fs:ops

