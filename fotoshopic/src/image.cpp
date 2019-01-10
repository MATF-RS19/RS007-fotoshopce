#include "headers/image_params.h"
#include "headers/image.h"


/*
* @brief Builds instance of Image.
*/
Image::Image(const cv::Mat& img, std::string filename)
	:	m_img(img),
		m_filename(std::move(filename)),
		m_type{image_type::color},
		m_param_list(1),
		m_index{0}
{}

/*
* @brief Destroys instance of Image
* TODO: Check if this is necessary [@milanilic332]
*/
Image::~Image() { m_img.release(); }

// TODO: Refactor function [@stefanpantic]
cv::Mat Image::get_current()
{
	cv::Mat new_image;
	m_img.copyTo(new_image);

	auto tl{m_param_list[m_index].corners[0]}, tr{m_param_list[m_index].corners[1]}, bl{m_param_list[m_index].corners[2]}, br{m_param_list[m_index].corners[3]};

	// TODO: Move ifs to function [@milanilic332]
	if (tl == image_corners::top_right && tr == image_corners::bottom_right &&
		bl == image_corners::top_left && br == image_corners::bottom_left) {
			cv::rotate(new_image, new_image, cv::ROTATE_90_COUNTERCLOCKWISE);
	}

	if (tl == image_corners::bottom_right && tr == image_corners::bottom_left &&
		bl == image_corners::top_right && br == image_corners::top_left) {
			cv::rotate(new_image, new_image, cv::ROTATE_90_COUNTERCLOCKWISE);
			cv::rotate(new_image, new_image, cv::ROTATE_90_COUNTERCLOCKWISE);
	}

	if (tl == image_corners::bottom_left && tr == image_corners::top_left &&
		bl == image_corners::bottom_right && br == image_corners::top_right) {
			cv::rotate(new_image, new_image, cv::ROTATE_90_CLOCKWISE);
	}

	if (tl == image_corners::bottom_right && tr == image_corners::top_right &&
		bl == image_corners::bottom_left && br == image_corners::top_left) {
			cv::rotate(new_image, new_image, cv::ROTATE_90_COUNTERCLOCKWISE);
			cv::flip(new_image, new_image, 1);
	}

	if (tl == image_corners::bottom_left && tr == image_corners::bottom_right &&
		bl == image_corners::top_left && br == image_corners::top_right) {
			cv::rotate(new_image, new_image, cv::ROTATE_90_COUNTERCLOCKWISE);
			cv::rotate(new_image, new_image, cv::ROTATE_90_COUNTERCLOCKWISE);
			cv::flip(new_image, new_image, 1);
	}
	if (tl == image_corners::top_left && tr == image_corners::bottom_left &&
		bl == image_corners::top_right && br == image_corners::bottom_right) {
			cv::rotate(new_image, new_image, cv::ROTATE_90_CLOCKWISE);
			cv::flip(new_image, new_image, 1);
	}

	if (tl == image_corners::top_right && tr == image_corners::top_left &&
		bl == image_corners::bottom_right && br == image_corners::bottom_left) {
			cv::flip(new_image, new_image, 1);
	}

	int brightness{m_param_list[m_index].adjustment_map[QString::fromStdString("Brightness")]};
	int contrast{m_param_list[m_index].adjustment_map[QString::fromStdString("Contrast")]};
	int blur{m_param_list[m_index].adjustment_map[QString::fromStdString("Blur")]};
	int sharpen{m_param_list[m_index].adjustment_map[QString::fromStdString("Sharpen")]};
	int vignette{m_param_list[m_index].adjustment_map[QString::fromStdString("Vignette")]};

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

	// Adjusting vignette effect to the image if needed (HACK)
	if(vignette) {
		QFile file(":/images/images/gaussian_filter.png");
		cv::Mat gaussian_filter;

		if(file.open(QIODevice::ReadOnly)){
			qint64 size = file.size();
			std::vector<uchar> buf(size);
			file.read((char*)buf.data(), size);
			gaussian_filter = imdecode(buf, cv::IMREAD_COLOR);
		}

		cv::resize(gaussian_filter, gaussian_filter, cv::Size(new_image.cols, new_image.rows));
		cv::add(gaussian_filter, cv::Scalar(150 - vignette*2.0, 150 - vignette*2.0, 150 - vignette*2.0), gaussian_filter);
		gaussian_filter.convertTo(gaussian_filter, CV_32FC3);
		new_image.convertTo(new_image, CV_32FC3);
		cv::multiply(gaussian_filter, cv::Scalar(1/255.0, 1/255.0, 1/255.0), gaussian_filter);
		cv::multiply(gaussian_filter, new_image, new_image);
		new_image.convertTo(new_image, CV_8UC3);
	}

	// Apply filter
	switch (m_param_list[m_index].filter)
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


	return new_image;
}
