#include "headers/image.h"


Image::Image(const cv::Mat& img, const std::string& fileName)
	: m_img(img), m_filename(fileName), m_type{1}
{
	ImageParams tmp;
	param_list.push_back(tmp);
	index = 0;
}

Image::Image()
	: m_img(cv::Mat()), m_filename(std::string())
{}

Image::~Image()
{
	m_img.release();
}

cv::Mat Image::get_current()
{
	cv::Mat new_image;
	m_img.copyTo(new_image);

	auto tl{param_list[index].corners[0]};
	auto tr{param_list[index].corners[1]};
	auto bl{param_list[index].corners[2]};
	auto br{param_list[index].corners[3]};

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

	int brightness{param_list[index].adjustment_map[QString::fromStdString("Brightness")]};
	int contrast{param_list[index].adjustment_map[QString::fromStdString("Contrast")]};
	int blur{param_list[index].adjustment_map[QString::fromStdString("Blur")]};
	int sharpen{param_list[index].adjustment_map[QString::fromStdString("Sharpen")]};
	int vignette{param_list[index].adjustment_map[QString::fromStdString("Vignette")]};

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

	cv::Mat im_gray;
	cv::cvtColor(new_image, im_gray, cv::COLOR_RGB2GRAY);
	cv::Mat im_color;
	cv::applyColorMap(im_gray, new_image, cv::COLORMAP_JET);

	return new_image;
}
