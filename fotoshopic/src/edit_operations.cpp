#include "opencv2/highgui/highgui.hpp"


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
		auto diff{m_to - m_from};

		switch(m_edit) {
			case fs::ops::basic_edits::brightness:
			{
				img.m_img.convertTo(img.m_img, -1, 1, diff);
				break;
			}
			case fs::ops::basic_edits::contrast:
			{
				if(diff < 0) {
					img.m_img.convertTo(img.m_img, -1, 0.9, 0);
				} else {
					img.m_img.convertTo(img.m_img, -1, 1.1, 0);
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
			default:
				break;
		}
	}

} // namespace fs::ops
