#pragma once

#include <iostream>

#include <QLabel>
#include <QMouseEvent>

class MouseLabel: public QLabel {
	Q_OBJECT
	public:
		explicit MouseLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
		~MouseLabel();
		std::pair<int, int> get_previous();
		std::pair<int, int> get_diff();

	signals:
		void clicked();
		void moved();
		void released();

	protected:
		void mousePressEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);

	private:
		bool m_pressed;
		std::pair<int, int> m_previous_pos;
		std::pair<int, int> m_diff;
};
