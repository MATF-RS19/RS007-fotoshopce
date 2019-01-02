#include "headers/image.h"

Image::Image(const cv::Mat& img, const std::string& fileName)
    : mImg(img), mFileName(fileName), mWidth(img.cols), mHeight(img.rows)
{}

Image::Image()
    : mImg(cv::Mat()), mFileName(std::string())
{}

Image::~Image()
{
    mImg.release();
}

Image::Image(const Image& im)
    : mImg(im.mImg), mFileName(im.mFileName),
      mCurrentZoom(im.mCurrentZoom), mType(im.mType),
      mMirrored(im.mMirrored), mWidth(im.mWidth),
      mHeight(im.mHeight)
{}

Image::Image(Image&& im)
    : mImg(std::move(im.mImg)), mFileName(std::move(im.mFileName)),
      mCurrentZoom(std::move(im.mCurrentZoom)), mType(std::move(im.mType)),
      mMirrored(std::move(im.mMirrored)), mWidth(std::move(im.mWidth)),
      mHeight(std::move(im.mHeight))
{}

Image Image::operator=(const Image& im)
{
    mImg = im.mImg;
    mFileName = im.mFileName;
    mCurrentZoom = im.mCurrentZoom;
    mType = im.mType;
    mMirrored = im.mMirrored;
    mWidth = im.mWidth;
    mHeight = im.mHeight;
    return *this;
}

Image Image::operator=(Image&& im)
{
    mImg = std::move(im.mImg);
    mFileName = std::move(im.mFileName);
    mCurrentZoom = std::move(im.mCurrentZoom);
    mType = std::move(im.mType);
    mMirrored = std::move(im.mMirrored);
    mWidth = std::move(im.mWidth);
    mHeight = std::move(im.mHeight);
    return *this;
}

//void Image::setType(int type)
//{
//    mType = type;
//}

//void Image::setCurrentZoom(double zoom)
//{
//    mCurrentZoom = zoom;
//}

//int Image::getType() const
//{
//    return mType;
//}

//double Image::getCurrentZoom() const
//{
//    return mCurrentZoom;
//}
