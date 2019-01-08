#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

#include <cmath>

#include "../headers/edit_operations.h"


namespace fs::ops
{
	// Basic edit operation constructor
	BasicEditOperation::BasicEditOperation(const Image &img, float from, float to, basic_edits edit)
		:	AbstractOperation(img),
			m_from{from},
			m_to{to},
			m_edit{edit}
		{}

	void BasicEditOperation::apply(Image &img)
	{
		double diff(static_cast<double>(m_to - m_from));

		switch(m_edit) {
			case fs::ops::basic_edits::brightness:
			{
				img.m_img.convertTo(img.m_img, -1, 1, diff);
				break;
			}
			case fs::ops::basic_edits::contrast:
			{
				if(diff < 0) {
					img.m_img.convertTo(img.m_img, -1, 0.5, 0);
				} else {
					img.m_img.convertTo(img.m_img, -1, 1.5, 0);
				}

				break;
			}
			case fs::ops::basic_edits::saturation:
			{
				img.m_img.convertTo(img.m_img, CV_8UC1, 1, diff);
				break;
			}
			case fs::ops::basic_edits::blur:
			{
				GaussianBlur(img.m_img, img.m_img, cv::Size(11, 11), std::abs(diff) / 8, 0.0,
							cv::BORDER_DEFAULT);

				double alpha = 2.5;
				double beta  = 1.0 - alpha;
				addWeighted(img.m_img, alpha, img.m_img, beta, 0.0, img.m_img);

				break;
			}
			case fs::ops::basic_edits::sharpen:
			{
				cv::Mat gaussblur;

				GaussianBlur(img.m_img, gaussblur, cv::Size(0, 0), std::abs(diff) / 8);
				addWeighted(img.m_img, 1.5, gaussblur, -0.5, 0, img.m_img);

				break;
			}
			case fs::ops::basic_edits::vignette:
			{
				double cols{static_cast<double>(img.m_img.cols / 2)}, rows{static_cast<double>(img.m_img.rows / 2)};
				double max_dis{(1.7 - (1.3 / 99 * (m_to - 99) + 1.5))* std::sqrt(cols * cols + rows * rows)}, temp;

				for (int i = 0; i < img.m_img.rows; ++i) {
					for (int j = 0; j < img.m_img.cols; ++j) {
						double dist{std::sqrt(std::pow((cols - j), 2) + std::pow((rows - i), 2))};
						temp = std::cos(dist / max_dis);
						temp *= temp;
						img.m_img.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>((img.m_img.at<cv::Vec3b>(i, j)[0]) * temp);
						img.m_img.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>((img.m_img.at<cv::Vec3b>(i, j)[1]) * temp );
						img.m_img.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>((img.m_img.at<cv::Vec3b>(i, j)[2]) * temp);
					}
				}

				break;
			}
			default:
				break;
		}
	}

} // namespace fs::ops
