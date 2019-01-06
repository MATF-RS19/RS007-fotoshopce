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
