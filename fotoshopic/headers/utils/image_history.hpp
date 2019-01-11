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
			inline Image current() const { return m_entries[m_position]; }
			void set_initial(const Image &img);
			void add_entry(const Image &img);
			Image undo();
			Image redo();

		/* Private member functions */
		private:
			void sync_entries();

		/* Private member variables */
		private:
			std::vector<Image> m_entries;
			size_t m_position;
	};

} // namespace rs:::utils
