#include "headers/utils/image_history.hpp"


namespace rs::utils
{

	/*
	* @brief Builds instance of image_history.
	*/
	image_history::image_history()
		:	m_position{0}
	{}

	/*
	* @brief Sets initial image.
	*/
	void image_history::set_initial(const image &img, const image_parameters &params)
	{
		m_entries.clear();
		m_entries.push_back({img, params});
		m_position = 0;
	}

	/*
	* @brief Add image entry to history.
	*/
	void image_history::add_entry(const image &img, const image_parameters &params)
	{
		if(m_position + 1 != int(m_entries.size())) {
			m_entries.erase(m_entries.begin() + int(m_position) + 1, m_entries.end());
		}

		m_entries.push_back({img, params});
		m_position += 1;
	}

	/*
	* @brief Undo an entry from history.
	*/
	std::pair<image, image_parameters> image_history::undo()
	{
		m_position = std::max(0, m_position - 1);
		return {image::set_parameters(m_entries[m_position].first, m_entries[m_position].second), m_entries[m_position].second};
	}

	/*
	* @brief Redo an entry from history.
	*/
	std::pair<image, image_parameters> image_history::redo()
	{
		m_position = std::min(int(m_entries.size() - 1), m_position + 1);
		return {image::set_parameters(m_entries[m_position].first, m_entries[m_position].second), m_entries[m_position].second};
	}

} // namespace rs::utils
