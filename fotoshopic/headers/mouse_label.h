#pragma once


#include <iostream>
#include <QLabel>
#include <QMouseEvent>


/*
* @brief Class that represents a mouse clickable/dragable label.
*/
class MouseLabel: public QLabel {
	Q_OBJECT

	/* Public member functions */
	public:
		explicit MouseLabel(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
		inline std::pair<int, int> get_previous() const { return m_previous_pos; }
		inline std::pair<int, int> get_diff() const { return m_diff; }

	/* Signals */
	signals:
		void clicked();
		void moved();
		void released();

	/* Protected member functions */
	protected:
		void mousePressEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);

	/* Private member variables */
	private:
		bool m_pressed;
		std::pair<int, int> m_previous_pos;
		std::pair<int, int> m_diff;
};
