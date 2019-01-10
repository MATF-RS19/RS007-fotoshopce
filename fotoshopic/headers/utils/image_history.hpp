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
		/* Private internal enum class */
		enum class entry_type
		{
			parameters,
			image,
		};

		/* Public member functions */
		public:
			explicit image_history(const Image &img);
			void add_entry(const Image &img);
			void add_entry(const ImageParams &params);
			Image undo();
			Image redo();

		/* Private member functions */
		private:
			void sync_entries();

		/* Private member variables */
		private:
			std::vector<Image> m_images;
			std::vector<ImageParams> m_params;
			std::vector<entry_type> m_types;
			size_t m_image_index, m_param_index, m_type_index;
	};

} // namespace rs:::utils
