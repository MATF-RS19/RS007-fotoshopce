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


#include <QPropertyAnimation>

#include "headers/section.h"


/* Declaring static variables */
size_t Section::uid_assigner{0};

/*
* @brief Builds instance of section.
*/
Section::Section(const QString & title, const int duration, QWidget* parent)
	:	QWidget(parent),
		m_duration(duration),
		m_open{false},
		m_uid{uid_assigner}
{
	uid_assigner += 1;

	m_toggle_button = new QToolButton(this);
    m_header_line = new QFrame(this);
    m_toggle_animation = new QParallelAnimationGroup(this);
    m_content_area = new QScrollArea(this);
    m_main_layout = new QGridLayout(this);

	m_toggle_button->setStyleSheet("QToolButton { border: none; }");
    m_toggle_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_toggle_button->setArrowType(Qt::ArrowType::RightArrow);
    m_toggle_button->setText(title);
    m_toggle_button->setCheckable(true);
    m_toggle_button->setChecked(false);

    m_header_line->setFrameShape(QFrame::HLine);
    m_header_line->setFrameShadow(QFrame::Sunken);
    m_header_line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    m_content_area->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_content_area->setMaximumHeight(0);
    m_content_area->setMinimumHeight(0);

    m_toggle_animation->addAnimation(new QPropertyAnimation(this, "minimumHeight"));
    m_toggle_animation->addAnimation(new QPropertyAnimation(this, "maximumHeight"));
    m_toggle_animation->addAnimation(new QPropertyAnimation(m_content_area, "maximumHeight"));

    m_main_layout->setVerticalSpacing(0);
    m_main_layout->setContentsMargins(0, 0, 0, 0);

	int row{0};
    m_main_layout->addWidget(m_toggle_button, row, 0, 1, 1, Qt::AlignLeft);
    m_main_layout->addWidget(m_header_line, row++, 2, 1, 1);
    m_main_layout->addWidget(m_content_area, row, 0, 1, 3);
    setLayout(m_main_layout);
}

/*
* @brief Sets layout of section (usually a HBox or VBox).
*/
void Section::setContentLayout(QLayout & contentLayout)
{
    delete m_content_area->layout();
    m_content_area->setLayout(&contentLayout);
	auto collapsedHeight{sizeHint().height() - m_content_area->maximumHeight()};
	auto contentHeight{contentLayout.sizeHint().height()};

    for (int i = 0; i < m_toggle_animation->animationCount() - 1; ++i)
    {
		QPropertyAnimation *SectionAnimation{static_cast<QPropertyAnimation*>(m_toggle_animation->animationAt(i))};
        SectionAnimation->setDuration(m_duration);
        SectionAnimation->setStartValue(collapsedHeight);
        SectionAnimation->setEndValue(collapsedHeight + contentHeight);
    }

	QPropertyAnimation *contentAnimation{static_cast<QPropertyAnimation*>(m_toggle_animation->animationAt(m_toggle_animation->animationCount() - 1))};
    contentAnimation->setDuration(m_duration);
    contentAnimation->setStartValue(0);
    contentAnimation->setEndValue(contentHeight);
}

/*
* @brief Colapses section if open.
*/
void Section::colapse()
{
	if(m_open) {
		m_toggle_button->setArrowType(Qt::ArrowType::RightArrow);
		m_toggle_animation->setDirection(QAbstractAnimation::Backward);
		m_toggle_animation->start();
		m_open = false;
	}
}

/*
* @brief Expands section if closed.
*/
void Section::expand()
{
	if(!m_open) {
		m_toggle_button->setArrowType(Qt::ArrowType::DownArrow);
		m_toggle_animation->setDirection(QAbstractAnimation::Forward);
		m_toggle_animation->start();
		m_open = true;
	}
}
