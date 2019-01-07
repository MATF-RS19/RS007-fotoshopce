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

cv::Mat Image::get_current() const
{
	cv::Mat tmp;
	m_img.copyTo(tmp);

	int tl{param_list[index].topleft_corner};
	int tr{param_list[index].topright_corner};
	int bl{param_list[index].bottomleft_corner};
	int br{param_list[index].bottomright_corner};

	// TODO: Move ifs to function [@milanilic332]
	if (tl == 1 && tr == 3 &&
		bl == 0 && br == 2) {
			cv::rotate(tmp, tmp, cv::ROTATE_90_COUNTERCLOCKWISE);
	}

	if (tl == 3 && tr == 2 &&
		bl == 1 && br == 0) {
			cv::rotate(tmp, tmp, cv::ROTATE_90_COUNTERCLOCKWISE);
			cv::rotate(tmp, tmp, cv::ROTATE_90_COUNTERCLOCKWISE);
	}

	if (tl == 2 && tr == 0 &&
		bl == 3 && br == 1) {
			cv::rotate(tmp, tmp, cv::ROTATE_90_CLOCKWISE);
	}

	if (tl == 3 && tr == 1 &&
		bl == 2 && br == 0) {
			cv::rotate(tmp, tmp, cv::ROTATE_90_COUNTERCLOCKWISE);
			cv::flip(tmp, tmp, 1);
	}

	if (tl == 2 && tr == 3 &&
		bl == 0 && br == 1) {
			cv::rotate(tmp, tmp, cv::ROTATE_90_COUNTERCLOCKWISE);
			cv::rotate(tmp, tmp, cv::ROTATE_90_COUNTERCLOCKWISE);
			cv::flip(tmp, tmp, 1);
	}
	if (tl == 0 && tr == 2 &&
		bl == 1 && br == 3) {
			cv::rotate(tmp, tmp, cv::ROTATE_90_CLOCKWISE);
			cv::flip(tmp, tmp, 1);
	}

	if (tl == 1 && tr == 0 &&
		bl == 3 && br == 2) {
			cv::flip(tmp, tmp, 1);
	}

	double contrast{param_list[index].m_contrast};
	double brightness{param_list[index].m_brightness};
	double blur{param_list[index].m_blur};
	double sharpen{param_list[index].m_sharpen};

	cv::multiply(tmp, cv::Scalar(contrast, contrast, contrast), tmp);
	cv::add(tmp, cv::Scalar(brightness, brightness, brightness), tmp);

	// TODO: Bluring for both
	if (blur - sharpen != 0.0) {
		bool blur_or_sharpen{blur > sharpen};
		double sigma{std::abs(blur - sharpen)};
		if (blur_or_sharpen) {
			cv::GaussianBlur(tmp, tmp, cv::Size(11, 11), sigma);
			cv::addWeighted(tmp, 2.5, tmp, -1.5, 0, tmp);
		} else {
			cv::Mat gaussian;
			cv::GaussianBlur(tmp, gaussian, cv::Size(0, 0), sigma);
			cv::addWeighted(tmp, 1.5, gaussian, -0.5, 0, tmp);
		}
	}

	return tmp;
}
