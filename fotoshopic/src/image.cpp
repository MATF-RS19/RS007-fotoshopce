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

	int tl{param_list[index].topleft_corner};
	int tr{param_list[index].topright_corner};
	int bl{param_list[index].bottomleft_corner};
	int br{param_list[index].bottomright_corner};

	// TODO: Move ifs to function [@milanilic332]
	if (tl == 1 && tr == 3 &&
		bl == 0 && br == 2) {
			cv::rotate(new_image, new_image, cv::ROTATE_90_COUNTERCLOCKWISE);
	}

	if (tl == 3 && tr == 2 &&
		bl == 1 && br == 0) {
			cv::rotate(new_image, new_image, cv::ROTATE_90_COUNTERCLOCKWISE);
			cv::rotate(new_image, new_image, cv::ROTATE_90_COUNTERCLOCKWISE);
	}

	if (tl == 2 && tr == 0 &&
		bl == 3 && br == 1) {
			cv::rotate(new_image, new_image, cv::ROTATE_90_CLOCKWISE);
	}

	if (tl == 3 && tr == 1 &&
		bl == 2 && br == 0) {
			cv::rotate(new_image, new_image, cv::ROTATE_90_COUNTERCLOCKWISE);
			cv::flip(new_image, new_image, 1);
	}

	if (tl == 2 && tr == 3 &&
		bl == 0 && br == 1) {
			cv::rotate(new_image, new_image, cv::ROTATE_90_COUNTERCLOCKWISE);
			cv::rotate(new_image, new_image, cv::ROTATE_90_COUNTERCLOCKWISE);
			cv::flip(new_image, new_image, 1);
	}
	if (tl == 0 && tr == 2 &&
		bl == 1 && br == 3) {
			cv::rotate(new_image, new_image, cv::ROTATE_90_CLOCKWISE);
			cv::flip(new_image, new_image, 1);
	}

	if (tl == 1 && tr == 0 &&
		bl == 3 && br == 2) {
			cv::flip(new_image, new_image, 1);
	}

	int brightness{param_list[index].adjustment_map[QString::fromStdString("Brightness")]};
	int contrast{param_list[index].adjustment_map[QString::fromStdString("Contrast")]};
	int blur{param_list[index].adjustment_map[QString::fromStdString("Blur")]};
	int sharpen{param_list[index].adjustment_map[QString::fromStdString("Sharpen")]};
	int vignette{param_list[index].adjustment_map[QString::fromStdString("Vignette")]};

	// Set initial brightness value
	double brightness_value{0};

	// Calculate the new brightness value if it changed
	if(brightness < 50) {
		brightness_value = (50 - brightness) * (-63 / 50);
	} else if(brightness > 50) {
		brightness_value = (brightness - 50) * (63 / 50);
	}

	// Setting the image brightness
	cv::add(new_image, cv::Scalar(brightness_value, brightness_value, brightness_value), new_image);

	// Set initial contrast value
	double contrast_value{1};

	// Calculate the new contrast value if it changed
	if(contrast < 50) {
		contrast_value = contrast * (0.9 / 50);
	} else if(contrast > 50) {
		contrast_value = contrast * (1.2 / 50);
	}

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

	return new_image;
}
