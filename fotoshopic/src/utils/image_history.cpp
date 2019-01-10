#include "headers/utils/image_history.hpp"


namespace rs::utils
{

	/*
	* @brief Builds instance of image_history
	*/
	image_history::image_history(const Image &img)
		:	m_images{img},
			m_types{entry_type::image},
			m_image_index{0},
			m_param_index{0},
			m_type_index{0}
	{}

	/*
	* @brief Add image entry to history.
	*/
	void image_history::add_entry(const Image &img)
	{
		m_images.push_back(img);
		m_types.push_back(entry_type::image);
		sync_entries();
	}

	/*
	* @brief Add parameter entry to history.
	*/
	void image_history::add_entry(const ImageParams &params)
	{
		m_params.push_back(params);
		m_types.push_back(entry_type::parameters);
		sync_entries();
	}

	/*
	* @brief Undo an entry from history.
	*/
	// TODO: Implement undo [@stefanpantic]
	Image image_history::undo()
	{
		return Image();
	}

	/*
	* @brief Redo an entry from history.
	*/
	// TODO: Implement redo [@stefanpantic]
	Image image_history::redo()
	{
		return Image();
	}

	/*
	* @brief Deletes entries from current index to end.
	*/
	void image_history::sync_entries()
	{
		if(m_type_index != m_types.size() - 1) {
			m_types.erase(m_types.begin() + int(m_type_index), m_types.end());
			m_type_index = m_types.size() - 1;
		} else {
			m_type_index++;
		}

		if(m_image_index != m_images.size() - 1) {
			m_images.erase(m_images.begin() + int(m_image_index), m_images.end());
			m_image_index = m_images.size() - 1;
		} else {
			m_image_index++;
		}

		if(m_param_index != m_params.size() - 1) {
			m_params.erase(m_params.begin() + int(m_param_index), m_params.end());
			m_param_index = m_params.size() - 1;
		} else {
			m_param_index++;
		}
	}

} // namespace rs::utils
