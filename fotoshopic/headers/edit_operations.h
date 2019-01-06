#pragma once


#include "abstract_operation.h"
#include "image.h"


namespace fs::ops
{

	/*
	* @breif Enum class representing color operations.
	*/
	enum class basic_edits
	{
		brightness,
		contrast,
		shadows,
		highlights,
		vignette,
		saturation,
		luminance,
		temperature,
		sharpen,
		blur,
	};


	/*
	* @brief Class representing basic edit operations.
	*/
	class BasicEditOperation : public AbstractOperation
	{
		public:
			explicit BasicEditOperation(const Image &img, float from, float to, basic_edits edit);
			virtual void apply(Image &img) override;

			inline float from() const { return m_from; }
			inline float to() const { return m_to; }
		private:
			float m_from, m_to;
			basic_edits m_edit;
	};


	/*
	* @brief Class representing advanced edit operations.
	*/
	class AdvancedEditOperation : public BasicEditOperation
	{
		public:
			explicit AdvancedEditOperation(const Image &img, float from, float to);
			virtual void apply(Image &img) override;
	};

} // namespace fs::ops
