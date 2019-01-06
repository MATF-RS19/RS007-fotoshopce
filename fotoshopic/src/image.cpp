#include "headers/image.h"

Image::Image(const cv::Mat& img, const std::string& fileName)
	: m_img(img), m_filename(fileName)
{}

Image::Image()
	: m_img(cv::Mat()), m_filename(std::string())
{}

Image::~Image()
{
	m_img.release();
}

Image::Image(const Image& im)
	: m_img(im.m_img), m_filename(im.m_filename), m_type(im.m_type)
{}

Image::Image(Image&& im)
	: m_img(std::move(im.m_img)), m_filename(std::move(im.m_filename)), m_type(std::move(im.m_type))
{}

Image Image::operator=(const Image& im)
{
	m_img = im.m_img;
	m_filename = im.m_filename;
	m_type = im.m_type;
	return *this;
}

Image Image::operator=(Image&& im)
{
	m_img = std::move(im.m_img);
	m_filename = std::move(im.m_filename);
	m_type = std::move(im.m_type);
	return *this;
}
