#include "headers/utils/image_history.hpp"


namespace rs::utils
{

	/*
	* @brief Sets initial image.
	*/
	void image_history::set_initial(const Image &img)
	{
		m_entries.clear();
		m_entries.push_back(img);
		m_position = 0;
	}

	/*
	* @brief Add image entry to history.
	*/
	void image_history::add_entry(const Image &img)
	{
		m_entries.push_back(img);
		sync_entries();
	}

	/*
	* @brief Undo an entry from history.
	*/
	Image image_history::undo()
	{
		return m_entries[0];
	}

	/*
	* @brief Redo an entry from history.
	*/
	Image image_history::redo()
	{
		return m_entries[0];
	}

	/*
	* @brief Deletes entries from current index to end.
	*/
	void image_history::sync_entries()
	{

	}

} // namespace rs::utils
