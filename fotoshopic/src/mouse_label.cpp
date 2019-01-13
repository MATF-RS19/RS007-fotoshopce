#include "headers/mouse_label.h"


MouseLabel::MouseLabel(QWidget* parent, Qt::WindowFlags)
	:	QLabel(parent),
		m_pressed{false},
		m_previous_pos{-1, -1},
		m_diff{-1, -1}
{}

std::pair<int, int> MouseLabel::get_previous() {
	return m_previous_pos;
}

std::pair<int, int> MouseLabel::get_diff() {
	return m_diff;
}

void MouseLabel::mousePressEvent(QMouseEvent* event) {
	if(event->buttons() & Qt::LeftButton) {
		m_pressed = true;
		m_previous_pos = {event->x(), event->y()};
		emit clicked();
	}
}

void MouseLabel::mouseMoveEvent(QMouseEvent* event) {
	if (m_pressed) {
		m_diff = {event->x() - m_previous_pos.first, event->y() - m_previous_pos.second};
		m_previous_pos = {event->x(), event->y()};
		emit moved();
	}
}

void MouseLabel::mouseReleaseEvent(QMouseEvent*) {
	m_pressed = false;
	m_previous_pos = {-1, -1};
	emit released();
}
