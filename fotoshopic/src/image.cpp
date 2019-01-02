#include "headers/image.h"

Image::Image(const cv::Mat& img, const std::string& fileName)
    : mImg(img), mFileName(fileName)
{}

Image::Image()
    : mImg(cv::Mat()), mFileName(std::string())
{}

Image::~Image()
{
    mImg.release();
}

Image::Image(const Image& im)
    : mImg(im.mImg), mFileName(im.mFileName), mType(im.mType)
{}

Image::Image(Image&& im)
    : mImg(std::move(im.mImg)), mFileName(std::move(im.mFileName)), mType(std::move(im.mType))
{}

Image Image::operator=(const Image& im)
{
    mImg = im.mImg;
    mFileName = im.mFileName;
    mType = im.mType;
    return *this;
}

Image Image::operator=(Image&& im)
{
    mImg = std::move(im.mImg);
    mFileName = std::move(im.mFileName);
    mType = std::move(im.mType);
    return *this;
}
