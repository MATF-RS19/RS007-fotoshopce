#include "headers/image_params.h"
#include "headers/image.h"


/*
* @brief Builds instance of Image.
*/
image::image(const cv::Mat& img)
	:	m_img(img),
		m_type{image_type::color},
		m_index{0}
{}

/*
* @brief Copies instance of Image.
*/
image::image(const image &img)
	:	m_type{img.m_type},
		m_index{img.m_index}
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
	cv::Mat new_image;
	img.m_img.copyTo(new_image);

	int brightness{params.adjustment_map[QString::fromStdString("Brightness")]};
	int contrast{params.adjustment_map[QString::fromStdString("Contrast")]};
	int blur{params.adjustment_map[QString::fromStdString("Blur")]};
	int sharpen{params.adjustment_map[QString::fromStdString("Sharpen")]};
	int vignette{params.adjustment_map[QString::fromStdString("Vignette")]};

	// Set the brightness value
	double brightness_value{brightness - 50.0};

	// Setting the image brightness
	cv::add(new_image, cv::Scalar(brightness_value, brightness_value, brightness_value), new_image);

	// Set the contrast value
	double contrast_value{contrast / 50.0};

	// Setting the image contrast
	cv::multiply(new_image, cv::Scalar(contrast_value, contrast_value, contrast_value), new_image);


	// Setting the image blur value if it changed
	if(blur) {
		cv::GaussianBlur(new_image, new_image, cv::Size(11, 11), blur);
		cv::addWeighted(new_image, 2.5, new_image, -1.5, 0, new_image);
	}

	// Setting the image sharpness value if it changed
	if(sharpen) {
		cv::Mat gaussian;
		cv::GaussianBlur(new_image, gaussian, cv::Size(11, 11), sharpen);
		cv::addWeighted(new_image, 1.5, gaussian, -0.5, 0, new_image);
	}

	// Adjusting vignette effect to the image if needed
	if(vignette) {
		double cols{static_cast<double>(new_image.cols / 2)}, rows{static_cast<double>(new_image.rows / 2)};
		double max_dis{(1.7 - (1.3 / 99 * (vignette - 99) + 1.5)) * std::sqrt(cols * cols + rows * rows)}, temp;

		for (int i = 0; i < new_image.rows; ++i) {
			for (int j = 0; j < new_image.cols; ++j) {
				double dist{std::sqrt(std::pow((cols - j), 2) + std::pow((rows - i), 2))};
				temp = std::cos(dist / max_dis);
				temp *= temp;
				new_image.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>((new_image.at<cv::Vec3b>(i, j)[0]) * temp);
				new_image.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>((new_image.at<cv::Vec3b>(i, j)[1]) * temp);
				new_image.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>((new_image.at<cv::Vec3b>(i, j)[2]) * temp);
			}
		}
	}

	// Apply filter
	switch (params.filter)
	{
		case filters::none:
			break;
		case filters::autumn:
			cv::cvtColor(new_image, new_image, cv::COLORMAP_AUTUMN);
			break;
		case filters::bone:
			cv::cvtColor(new_image, new_image, cv::COLORMAP_BONE);
			break;
		case filters::jet:
			cv::cvtColor(new_image, new_image, cv::COLORMAP_JET);
			break;
		case filters::winter:
			cv::cvtColor(new_image, new_image, cv::COLORMAP_WINTER);
			break;
		case filters::rainbow:
			cv::cvtColor(new_image, new_image, cv::COLORMAP_RAINBOW);
			break;
		case filters::ocean:
			cv::cvtColor(new_image, new_image, cv::COLORMAP_OCEAN);
			break;
		case filters::summer:
			cv::cvtColor(new_image, new_image, cv::COLORMAP_SUMMER);
			break;
		case filters::spring:
			cv::cvtColor(new_image, new_image, cv::COLORMAP_SPRING);
			break;
		case filters::cool:
			cv::cvtColor(new_image, new_image, cv::COLORMAP_COOL);
			break;
		case filters::hsv:
			cv::cvtColor(new_image, new_image, cv::COLORMAP_HSV);
			break;
		case filters::pink:
			cv::cvtColor(new_image, new_image, cv::COLORMAP_PINK);
			break;
		case filters::hot:
			cv::cvtColor(new_image, new_image, cv::COLORMAP_HOT);
			break;
	}


	return image(new_image);
}
