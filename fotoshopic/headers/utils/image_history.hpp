#pragma once


#include <iostream>
#include <vector>

#include "headers/image.h"
#include "headers/image_params.h"


namespace rs::utils
{

	/*
	* @brief Class that implements history of image states.
	*/
	class image_history
	{

		/* Public member functions */
		public:
			explicit image_history();
			inline image current_image() const { return image::set_parameters(m_entries[m_position].first, m_entries[m_position].second); }
			inline const image &current_template() const { return m_entries[m_position].first; }
			inline const image_parameters &current_parameters() const { return m_entries[m_position].second; }
			inline bool undoable() const { return m_position > 0; }
			inline bool redoable() const { return m_position + 1< int(m_entries.size()); }

			void set_initial(const image &img, const image_parameters &params);
			void add_entry(const image &img, const image_parameters &params = image_parameters());
			std::pair<image, image_parameters> undo();
			std::pair<image, image_parameters> redo();

		/* Private member variables */
		private:
			std::vector<std::pair<image, image_parameters>> m_entries;
			int m_position;
	};

} // namespace rs:::utils
