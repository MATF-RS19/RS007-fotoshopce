#include "headers/utils/image_history.hpp"


namespace rs::utils
{

	/*
	* @brief Sets initial image.
	*/
	void image_history::set_initial(const Image &img)
	{
		m_images.push_back(img);
		m_types.push_back(entry_type::image);
		m_params.push_back(ImageParams());
		m_image_index = m_param_index = m_type_index = 0;
	}


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
	* @brief Fetches the current image.
	*/
	Image image_history::current() const
	{
		if(entry_type::image == m_types[m_type_index]) {
			std::cerr << m_image_index << " " << m_images.size() << std::endl;
			return m_original;
		} else {
			return Image::set_parameters(m_original, m_params[m_param_index]);
		}
	}


	/*
	* @brief Undo an entry from history.
	*/
	Image image_history::undo()
	{
		m_type_index -= 1;

		if(entry_type::image == m_types[m_type_index]) {
			m_image_index -= 1;
			return m_images[m_image_index];
		} else {
			m_param_index -= 1;
			return Image::set_parameters(m_original, m_params[m_param_index]);
		}
	}

	/*
	* @brief Redo an entry from history.
	*/
	Image image_history::redo()
	{
		m_type_index += 1;

		if(entry_type::image == m_types[m_type_index]) {
			m_image_index += 1;
			return m_images[m_image_index];
		} else {
			m_param_index += 1;
			return Image::set_parameters(m_original, m_params[m_param_index]);
		}
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
			m_type_index += 1;
		}

		if(m_image_index != m_images.size() - 1) {
			m_images.erase(m_images.begin() + int(m_image_index), m_images.end());
			m_image_index = m_images.size() - 1;
		} else {
			m_image_index += 1;
		}

		if(m_param_index != m_params.size() - 1) {
			m_params.erase(m_params.begin() + int(m_param_index), m_params.end());
			m_param_index = m_params.size() - 1;
		} else {
			m_param_index += 1;
		}
	}

} // namespace rs::utils
