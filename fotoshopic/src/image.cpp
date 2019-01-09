#include "headers/image.h"

Image::Image(const cv::Mat& img, const std::string& fileName)
	: m_img(img), m_filename(fileName), m_type{1}
{
	image_params tmp;
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

	// TODO: Fix it [d]
	int contrast_from{param_list[index].m_adjustment_map[QString::fromStdString("Contrast")].first};
	int brightness_from{param_list[index].m_adjustment_map[QString::fromStdString("Brightness")].first};
	int blur_from{param_list[index].m_adjustment_map[QString::fromStdString("Blur")].first};
	int sharpen_from{param_list[index].m_adjustment_map[QString::fromStdString("Sharpen")].first};
	int vignette_from{param_list[index].m_adjustment_map[QString::fromStdString("Vignette")].first};

	int contrast_to{param_list[index].m_adjustment_map[QString::fromStdString("Contrast")].second};
	int brightness_to{param_list[index].m_adjustment_map[QString::fromStdString("Brightness")].second};
	int blur_to{param_list[index].m_adjustment_map[QString::fromStdString("Blur")].second};
	int sharpen_to{param_list[index].m_adjustment_map[QString::fromStdString("Sharpen")].second};
	int vignette_to{param_list[index].m_adjustment_map[QString::fromStdString("Vignette")].second};

	// Setting the image contrast value if it changed
	if(contrast_to - contrast_from < 0) {
		new_image.convertTo(new_image, -1, 0.9, 0);
	} else if(contrast_to - contrast_from > 0) {
		new_image.convertTo(new_image, -1, 1.1, 0);
	}

	// Setting the image brightness value if it changed
	if(brightness_to - brightness_from) {
		new_image.convertTo(new_image, -1, 1, brightness_to - brightness_from);
	}

	// Setting the image blur value if it changed
	if(blur_to - blur_from > 0) {
		double sigma(std::abs(blur_to - blur_from));
		cv::GaussianBlur(new_image, new_image, cv::Size(11, 11), sigma);
		cv::addWeighted(new_image, 2.5, new_image, -1.5, 0, new_image);
	} else {
		// TODO: Decrease blur
	}

	// Setting the image sharpness value if it changed
	if(sharpen_to - sharpen_from > 0) {
		cv::Mat gaussian;
		double sigma(std::abs(sharpen_to - sharpen_from));
		cv::GaussianBlur(new_image, gaussian, cv::Size(0, 0), sigma);
		cv::addWeighted(new_image, 1.5, gaussian, -0.5, 0, new_image);
	} else {
		// TODO: Decrease sharpness
	}

	// Adjusting vignette effect to the image if needed
	if(vignette_to - vignette_from > 0) {
		double cols{static_cast<double>(new_image.cols / 2)}, rows{static_cast<double>(new_image.rows / 2)};
		double max_dis{(1.7 - (1.3 / 99 * (vignette_to - 99) + 1.5)) * std::sqrt(cols * cols + rows * rows)}, temp;

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
	} else {
		// TODO: Decrease vignette effect
	}

	return new_image;
}
