/*
	Elypson/qt-collapsible-section
	(c) 2016 Michael A. Voelkel - michael.alexander.voelkel@gmail.com

	This file is part of Elypson/qt-collapsible section.

	Elypson/qt-collapsible-section is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Elypson/qt-collapsible-section is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Elypson/qt-collapsible-section. If not, see <http://www.gnu.org/licenses/>.
*/    

#pragma once


#include <QFrame>
#include <QGridLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>
#include <QWidget>


/*
* @brief Class that implements a collapsable widget Code was taken from GitHub and modified for our use case.
*/
class Section : public QWidget {
	Q_OBJECT

	// Public member functions
	public:
		explicit Section(const QString & title = "", const int duration = 100, QWidget* parent = nullptr);

		inline bool operator==(const Section &other) const { return m_uid == other.m_uid; }
		inline bool operator!=(const Section &other) const { return !(*this == other); }

		inline bool open() const { return m_open; }
		inline QToolButton *toggle() { return m_toggle_button; }
		inline size_t uid() const { return m_uid; }

		void expand();
		void colapse();
		void setContentLayout(QLayout & contentLayout);

	// Private member variables
	private:
		QGridLayout* m_main_layout;
		QToolButton* m_toggle_button;
		QFrame* m_header_line;
		QParallelAnimationGroup* m_toggle_animation;
		QScrollArea* m_content_area;
		int m_duration;
		bool m_open;
		size_t m_uid;
		static size_t uid_assigner;
};
