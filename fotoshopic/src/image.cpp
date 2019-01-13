#include <QFile>

#include "headers/image_params.h"
#include "headers/image.h"


/*
* @brief Builds instance of Image.
*/
image::image(const cv::Mat& img)
	:	m_img(img)
{}

/*
* @brief Copies instance of Image.
*/
image::image(const image &img)
{
	img.m_img.copyTo(m_img);
}

/*
* @brief Destroys instance of Image
* TODO: Check if this is necessary [@milanilic332]
*/
image::~image() { m_img.release(); }

/*
* @brief Transforms image using parameters.
*/
// TODO: Refactor function [@stefanpantic]
image image::set_parameters(image img, image_parameters params)
{
	// Resizes image into correct size
	int width{params.size.first};
	int height{params.size.second};
	cv::resize(img.m_img, img.m_img, cv::Size(width, height));

	int brightness{params.adjustment_map[QString::fromStdString("Brightness")]},
		contrast{params.adjustment_map[QString::fromStdString("Contrast")]},
		saturation{params.adjustment_map[QString::fromStdString("Saturation")]},
		blur{params.adjustment_map[QString::fromStdString("Blur")]},
		sharpen{params.adjustment_map[QString::fromStdString("Sharpen")]},
		vignette{params.adjustment_map[QString::fromStdString("Vignette")]},
		hue{params.adjustment_map[QString::fromStdString("Hue")]},
		value{params.adjustment_map[QString::fromStdString("Value")]};

	// Set brightness
	double brightness_v{brightness - 50.0};
	cv::add(img.m_img, cv::Scalar(brightness_v, brightness_v, brightness_v), img.m_img);

	// Set contrast
	double contrast_v{contrast / 50.0};
	cv::multiply(img.m_img, cv::Scalar(contrast_v, contrast_v, contrast_v), img.m_img);

	// Setting the image blur value if it changed
	if(blur) {
		cv::GaussianBlur(img.m_img, img.m_img, cv::Size(11, 11), blur / 25.0);
		cv::addWeighted(img.m_img, 2.5, img.m_img, -1.5, 0, img.m_img);
	}

	// Setting the image sharpness value if it changed
	if(sharpen) {
		cv::Mat gaussian;
		cv::GaussianBlur(img.m_img, gaussian, cv::Size(11, 11), sharpen);
		cv::addWeighted(img.m_img, 1.5, gaussian, -0.5, 0, img.m_img);
	}

	// Adjusting vignette effect to the image if needed (HACK)
	if(vignette) {
		QFile file(":/images/images/gaussian_filter.png");
		cv::Mat gaussian_filter;

		if(file.open(QIODevice::ReadOnly)){
			qint64 size(file.size());
			std::vector<uchar> buf(static_cast<size_t>(size));
			file.read((char*)buf.data(), size);
			gaussian_filter = imdecode(buf, cv::IMREAD_COLOR);
		}

		cv::resize(gaussian_filter, gaussian_filter, cv::Size(img.m_img.cols, img.m_img.rows));
		cv::add(gaussian_filter, cv::Scalar(150 - vignette * 2.0, 150 - vignette * 2.0, 150 - vignette * 2.0), gaussian_filter);
		gaussian_filter.convertTo(gaussian_filter, CV_32FC3);
		img.m_img.convertTo(img.m_img, CV_32FC3);
		cv::multiply(gaussian_filter, cv::Scalar(1 / 255.0, 1 / 255.0, 1 / 255.0), gaussian_filter);
		cv::multiply(gaussian_filter, img.m_img, img.m_img);
		img.m_img.convertTo(img.m_img, CV_8UC3);
	}

	// Apply filter
	switch (params.filter)
	{
		case filters::none:
			break;
		case filters::autumn:
			image::apply_filter(img.m_img, cv::COLORMAP_AUTUMN);
			break;
		case filters::bone:
			image::apply_filter(img.m_img, cv::COLORMAP_BONE);
			break;
		case filters::jet:
			image::apply_filter(img.m_img, cv::COLORMAP_JET);
			break;
		case filters::winter:
			image::apply_filter(img.m_img, cv::COLORMAP_WINTER);
			break;
		case filters::rainbow:
			image::apply_filter(img.m_img, cv::COLORMAP_RAINBOW);
			break;
		case filters::ocean:
			image::apply_filter(img.m_img, cv::COLORMAP_OCEAN);
			break;
		case filters::summer:
			image::apply_filter(img.m_img, cv::COLORMAP_SUMMER);
			break;
		case filters::spring:
			image::apply_filter(img.m_img, cv::COLORMAP_SPRING);
			break;
		case filters::cool:
			image::apply_filter(img.m_img, cv::COLORMAP_COOL);
			break;
		case filters::hsv:
			image::apply_filter(img.m_img, cv::COLORMAP_HSV);
			break;
		case filters::pink:
			image::apply_filter(img.m_img, cv::COLORMAP_PINK);
			break;
		case filters::hot:
			image::apply_filter(img.m_img, cv::COLORMAP_HOT);
			break;
	}

	switch (params.img_type)
	{
		case image_type::grayscale:
			cv::cvtColor(img.m_img, img.m_img, cv::COLOR_RGB2GRAY);
			break;
		case image_type::color:
			break;
	}

	// Set the hue value
	double hue_v{hue / 50.0};

	// Set the saturation value
	double saturation_v{saturation / 50.0};

	// Set the value value
	double value_v{value / 50.0};

	// Adjust image color settings needed
	if(hue_v != 1.0 || saturation_v != 1.0 || value_v != 1.0) {
		cv::cvtColor(img.m_img, img.m_img, cv::COLOR_RGB2HSV);
		cv::multiply(img.m_img, cv::Scalar(hue_v, saturation_v, value_v), img.m_img);
		cv::cvtColor(img.m_img, img.m_img, cv::COLOR_HSV2RGB);
	}

	return img;
}

/*
* @brief Applies filter to image.
*/
void image::image::apply_filter(cv::Mat &img, int filter)
{
	cv::cvtColor(img, img, cv::COLOR_RGB2GRAY);
	cv::applyColorMap(img, img, filter);
}
