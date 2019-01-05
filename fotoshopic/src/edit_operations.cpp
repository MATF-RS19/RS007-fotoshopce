#include "opencv2/highgui/highgui.hpp"


#include "../headers/edit_operations.h"


namespace fs::ops
{
	// Basic edit operation constructor
	BasicEditOperation::BasicEditOperation(float from, float to, basic_edits edit)
		:	m_from{from},
			m_to{to},
			m_edit{edit}
		{}

	void BasicEditOperation::apply(Image &img)
	{
		auto diff{m_to - m_from};

		switch(m_edit) {
			case fs::ops::basic_edits::brightness:
			{
				auto const_beta{1.0};
				auto beta{const_beta * diff};

				img.mImg = img.mImg + cv::Scalar(beta, beta, beta);
				break;
			}
			case fs::ops::basic_edits::contrast:
			{
				auto alpha{0.0};
				if(diff < 0) {
					auto const_alpha{0.001f};
					alpha = const_alpha * diff;
				} else {
					auto const_alpha{1.0};
					alpha = const_alpha * diff;
				}

				img.mImg = alpha * img.mImg;

				break;
			}
			default:
				break;
		}
	}


	void BasicEditOperation::invert(Image &img)
	{
		std::swap(m_to, m_from);
		apply(img);
	}

} // namespace fs::ops
