#include <iostream>

#include "ui_mainwindow.h"
#include "headers/mainwindow.h"
#include "headers/utils.h"
#include "headers/image.h"
#include "headers/section.h"
#include "headers/mouse_label.h"


/*
* @brief Builds instance of MainWindow.
*/
MainWindow::MainWindow(QWidget *parent)
	:	QMainWindow(parent),
		ui{new Ui::MainWindow},
		m_lb_image{new MouseLabel},
		m_has_image{false},
		m_slider_index{0},
		m_slider_values(1),
		m_filter_filenames{qstring_pair(":/icons/icons/filters/autumn.png", "Autumn"),
						   qstring_pair(":/icons/icons/filters/bone.png", "Bone"),
						   qstring_pair(":/icons/icons/filters/jet.png", "Jet"),
						   qstring_pair(":/icons/icons/filters/winter.png", "Winter"),
						   qstring_pair(":/icons/icons/filters/rainbow.png", "Rainbow"),
						   qstring_pair(":/icons/icons/filters/ocean.png", "Ocean"),
						   qstring_pair(":/icons/icons/filters/summer.png", "Summer"),
						   qstring_pair(":/icons/icons/filters/spring.png", "Spring"),
						   qstring_pair(":/icons/icons/filters/cool.png", "Cool"),
						   qstring_pair(":/icons/icons/filters/hsv.png", "HSV"),
						   qstring_pair(":/icons/icons/filters/pink.png", "Pink"),
						   qstring_pair(":/icons/icons/filters/hot.png", "Hot")}
{
	ui->setupUi(this);

	connect(m_lb_image, SIGNAL(clicked()), this, SLOT(on_label_clicked()));
	connect(m_lb_image, SIGNAL(moved()), this, SLOT(on_label_moved()));
	connect(m_lb_image, SIGNAL(released()), this, SLOT(on_label_released()));

    // Disable spacing
	// TODO: Adjust sidebar and image label size [@milanilic332]
    auto *hlMain{new QHBoxLayout};
	ui->mainContainer->setLayout(hlMain);
	ui->mainContainer->setMaximumWidth(700);
    hlMain->setStretchFactor(hlMain, 30);

	// Set side pannel alignment
	ui->hlSide->setAlignment(Qt::AlignTop);

	// Create basic photo adjustment sliders
	auto basic_sliders{create_section("Basic settings", {"Brightness", "Contrast", "Saturation"})};
	capture_sliders(basic_sliders);
	slider_operation(basic_sliders, "Brightness");
	slider_operation(basic_sliders, "Contrast");
	slider_operation(basic_sliders, "Saturation");

	// Create advanced photo adjustment sliders
	auto advanced_sliders{create_section("Advanced settings", {"Sharpen", "Vignette", "Blur"})};
	capture_sliders(advanced_sliders);
	slider_operation(advanced_sliders, "Sharpen", 0);
	slider_operation(advanced_sliders, "Vignette", 0);
	slider_operation(advanced_sliders, "Blur", 0);

	// TODO: Add color selection [@dijana-z]
	// Create individual color photo adjustment sliders
    auto color_individual_sliders{create_section("Individual color settings", {"Hue", "Saturation", "Luminance"}, 3)};
	capture_sliders(color_individual_sliders.first);

	// Create filter section
	m_filter_buttons = create_section("Filters");

	for(size_t i = 0; i < m_filter_buttons.size(); ++i)
	{
		m_filter_buttons[i].first->setIcon(QIcon(m_filter_filenames[i].first));
		m_filter_buttons[i].first->setIconSize(QSize(65, 65));
		m_filter_buttons[i].first->setToolTip(QString::fromStdString(m_filter_filenames[i].second));
	}

	// Setting Widget params
	m_lb_image->setMinimumSize(600, 600);
	m_lb_image->setMaximumSize(600, 600);

	// Future use for cropping
	m_lb_image->setMouseTracking(true);
	m_lb_image->setStyleSheet("QLabel { background-color : #CCCCCC; }");
	m_lb_image->setAlignment(Qt::AlignCenter);

	// Add image to main widget
    hlMain->addWidget(m_lb_image);

	// Make only one section open at any given moment
	for(auto *section : m_sections)
	{
		QObject::connect(section->toggle(), &QToolButton::clicked, [section, this](auto &&clicked) {
			static_cast<void>(clicked);

			if(!section->open()) {
				for(auto *e : m_sections)
				{
					if(*e != *section) {
						e->colapse();
					}
				}
			}

			if(!section->open()) {
				section->expand();
			} else {
				section->colapse();
			}
		});
	}
}

/*
* @brief Destroys MainWindow.
*/
MainWindow::~MainWindow()
{
	delete ui;
}

/*
* @brief Display image according to its type (either RGB or grayscale).
*/
void MainWindow::show_image()
{
	if (m_has_image) {
		cv::Mat current{m_image_list[m_image_index].get_current()};
		// Getting current slice thats showing
		cv::Rect myROI(m_current_left, m_current_top, m_current_right - m_current_left, m_current_bottom - m_current_top);
		cv::Mat croppedImage = current(myROI);

		if (image_type::grayscale == m_image_list[m_image_index].m_type) {
			cv::cvtColor(croppedImage, croppedImage, cv::COLOR_BGR2GRAY);
			m_lb_image->setPixmap(QPixmap::fromImage(QImage(croppedImage.data, croppedImage.cols, croppedImage.rows, int(croppedImage.step), QImage::Format_Indexed8)));
		} else if (image_type::color == m_image_list[m_image_index].m_type) {
			cv::cvtColor(croppedImage, croppedImage, cv::COLOR_BGR2RGB);
			m_lb_image->setPixmap(QPixmap::fromImage(QImage(croppedImage.data, croppedImage.cols, croppedImage.rows, int(croppedImage.step), QImage::Format_RGB888)));
		}
	} else {
		m_lb_image->clear();
	}
}

/*
* @brief Save image according to its type (either RGB or grayscale).
*/
void MainWindow::save_image(const std::string& filename)
{
	try {
		cv::Mat current{m_image_list[m_image_index].get_current()};
		if (image_type::grayscale == m_img.m_type) {
			cv::cvtColor(current, current, cv::COLOR_BGR2GRAY);
		} else if (image_type::color == m_img.m_type) {
			cv::cvtColor(current, current, cv::COLOR_BGR2RGB);
		}

		cv::imwrite(filename, current);
	} catch (...) {
		QMessageBox::warning(this, "Warning", "Cannot save file" + QString::fromStdString(filename));
	}
}

/*
* @brief Save passed sliders into map.
*/
void MainWindow::capture_sliders(const qstring_map<QSlider*> sliders)
{
	for(auto &&slider_pair : sliders) {
		m_sliders.emplace(slider_pair);
	}
}


/*
* @brief Set initial slider value and define slider movement trigger.
*/
void MainWindow::slider_operation(qstring_map<QSlider*> sliders, const QString &key, int value)
{
	sliders[key]->setSliderPosition(value);
	sliders[key]->setTracking(false);
	m_slider_values.front()[key] = value;

	QObject::connect(sliders[key], &QSlider::valueChanged, [this, key](auto &&e) {
		if(m_has_image) {
			ImageParams params{m_image_list[m_image_index].m_param_list[m_image_list[m_image_index].m_index]};
			m_image_list[m_image_index].m_param_list.push_back(params);
			m_image_list[m_image_index].m_index++;
			m_image_list[m_image_index].m_param_list[m_image_list[m_image_index].m_index].adjustment_map[key] = e;

			m_slider_values.push_back(m_image_list[m_image_index].m_param_list[m_image_list[m_image_index].m_index].adjustment_map);
			m_slider_index++;

			delete_after_redo();
			show_image();
		} else {
			QMessageBox::warning(this, "Warning", "No image to adjust.");
		}
	});
}

/*
* @brief Slot for Open signal.
*
* @note Emits a warning.
*/
void MainWindow::on_action_Open_triggered()
{
    if(m_has_image) {
        auto reply{QMessageBox::question(this, "Warning", "All unsaved changes will be lost. Do you wish to proceed?", QMessageBox::Yes | QMessageBox::No)};
        if(QMessageBox::No == reply) {
            return;
        }
    }

	m_image_list.clear();

	QString filename{QFileDialog::getOpenFileName(this, "Open file")};
	try {
		m_image_list.clear();
		m_slider_values.clear();
		m_image_index = m_slider_index = 0;
		cv::Mat current = cv::imread(filename.toStdString());

		Image img{current, filename.toStdString()};
		m_image_list.push_back(img);
        m_has_image = true;

		update_edges(current);

		show_image();
		setWindowTitle(filename);
	} catch (...) {
		QMessageBox::warning(this, "Warning", "Cannot open file" + filename);
	}
}

/*
* @brief Create drop-down slider section.
*/
qstring_map<QSlider*> MainWindow::create_section(QString name, const std::vector<QString> &contents)
{
    Section *section{new Section(name, 300, this)};
	m_sections.push_back(section);
	ui->hlSide->addWidget(section);

    auto *vbox{new QVBoxLayout()};
	qstring_map<QSlider*> sliders;

	for(auto &&e : contents)
	{
        vbox->addWidget(new QLabel(e, section));
		sliders[e] = new QSlider(Qt::Horizontal, section);
		vbox->addWidget(sliders[e]);
	}

    section->setContentLayout(*vbox);
    return sliders;
}

/*
 * @brief Create drop-down filter section.
 */
std::vector<std::pair<QPushButton*, filters>> MainWindow::create_section(QString name)
{
	Section *section{new Section(name, 300, this)};
	m_sections.push_back(section);
	ui->hlSide->addWidget(section);

	auto *vbox{new QVBoxLayout};
	vbox->setAlignment(Qt::AlignTop);

	std::vector<std::pair<QPushButton*, filters>> filter_buttons;

	std::vector<std::vector<filters>> filters{{filters::autumn, filters::bone, filters::jet},
											  {filters::winter, filters::rainbow, filters::ocean},
											  {filters::summer, filters::spring, filters::cool},
											  {filters::hsv, filters::pink, filters::hot}};

	for(size_t i = 0; i < 4; ++i)
	{
		auto *widget{new QWidget(this)};
		auto *hbox{new QHBoxLayout};
		hbox->setAlignment(Qt::AlignCenter);
		widget->setLayout(hbox);
		vbox->addWidget(widget);

		for(size_t j = 0; j < 3; ++j)
		{
			auto *button{new QPushButton};
			hbox->addWidget(button);
			filter_buttons.push_back({button, filters[i][j]});
			auto b_filter{filters[i][j]};
			button->setCheckable(true);
			button->setChecked(false);

			QObject::connect(button, &QPushButton::clicked, [button, b_filter, this](auto &&e) {
				if(m_has_image) {
					ImageParams params{m_image_list[m_image_index].m_param_list[m_image_list[m_image_index].m_index]};
					for(auto &&e : m_filter_buttons) {
						if(e.first->isChecked() && e.first != button) {
							e.first->setChecked(false);
						}
					}
					params.filter = e ? b_filter : filters::none;
					m_image_list[m_image_index].m_param_list.push_back(params);
					m_image_list[m_image_index].m_index++;
					delete_after_redo();
					show_image();
				} else {
					QMessageBox::warning(this, "Warning", "No image to adjust.");
				}
			});
		}
	}

	section->setContentLayout(*vbox);

	return filter_buttons;
}

/*
* @brief Create drop-down section with individual color selection.
*/
std::pair<qstring_map<QSlider*>, QButtonGroup*> MainWindow::create_section(QString name, const std::vector<QString> &contents, int buttons, bool select_one)
{
    Section *section{new Section(name, 300, this)};
	m_sections.push_back(section);
    ui->hlSide->addWidget(section);

    auto *rbuttons{new QWidget(this)};

    auto *hbox{new QHBoxLayout};
    hbox->setAlignment(Qt::AlignCenter);
    auto *toggle_group{new QButtonGroup(hbox)};

    for(int i = 0; i < buttons; ++i)
    {
        auto *rbutton{new QRadioButton};
        hbox->addWidget(rbutton);
        toggle_group->addButton(rbutton);
    }

    rbuttons->setLayout(hbox);

    auto *vbox{new QVBoxLayout()};
	qstring_map<QSlider*> sliders;
    vbox->addWidget(rbuttons);

    for(auto &&e : contents)
    {
        vbox->addWidget(new QLabel(e, section));
		sliders[e] = new QSlider(Qt::Horizontal, section);
		vbox->addWidget(sliders[e]);
    }

	if(buttons && select_one) {
		toggle_group->buttons()[0]->setChecked(true);
	}

    section->setContentLayout(*vbox);
    return {sliders, toggle_group};
}

/*
* @brief Slot for Save signal.
*/
void MainWindow::on_action_Save_triggered()
{
	if (m_has_image) {
		save_image(m_image_list[m_image_index].m_filename);
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded");
	}
}

/*
* @brief Save and name current image.
*/
void MainWindow::on_action_SaveAs_triggered()
{
	if (m_has_image) {
		QString file_name{QFileDialog::getSaveFileName(this, "Save as...")};
		save_image(file_name.toStdString());
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded");
	}
}

// TODO: Fix zooming, implement dragging while zoomed [@milanilic332]
// TODO: Check if image is too large or small
/*
* @brief TODO: description
*/
void MainWindow::on_action_ZoomIn_triggered()
{
	if (m_has_image) {
		cv::Mat current{m_image_list[m_image_index].get_current()};
		cv::resize(current, current, cv::Size(), 1.1, 1.1);

		update_edges(current);

		Image img{current, m_image_list[m_image_index].m_filename};
		m_image_list.push_back(img);
		m_image_index++;
		delete_after_redo();
		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded");
	}
}

// TODO: Fix zooming, implement dragging while zoomed [@milanilic332]
/*
* @brief TODO: description
*/
void MainWindow::on_action_ZoomOut_triggered()
{
	if (m_has_image) {
		cv::Mat current{m_image_list[m_image_index].get_current()};
		cv::resize(current, current, cv::Size(), 0.9, 0.9);

		update_edges(current);

		Image img{current, m_image_list[m_image_index].m_filename};
		m_image_list.push_back(img);
		m_image_index++;
		delete_after_redo();
		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded");
	}
}

/*
* @brief Convert image to its mirror reflection.
*/
void MainWindow::on_action_Mirror_triggered()
{
	if (m_has_image) {
		ImageParams params{m_image_list[m_image_index].m_param_list[m_image_list[m_image_index].m_index]};
		std::swap(params.corners[0], params.corners[1]);
		std::swap(params.corners[2], params.corners[3]);
		m_image_list[m_image_index].m_param_list.push_back(params);
		m_image_list[m_image_index].m_index++;
		delete_after_redo();
		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded");
	}
}

/*
* @brief Rotate image counterclockwise.
*/
void MainWindow::on_action_Rotate_left_triggered()
{
    if (m_has_image) {
		ImageParams params{m_image_list[m_image_index].m_param_list[m_image_list[m_image_index].m_index]};
		auto a{params.corners[0]}, b{params.corners[1]}, c{params.corners[2]}, d{params.corners[3]};
		params.corners[0] = b;
		params.corners[1] = d;
		params.corners[2] = a;
		params.corners[3] = c;
		m_image_list[m_image_index].m_param_list.push_back(params);
		m_image_list[m_image_index].m_index++;
		cv::Mat current = m_image_list[m_image_index].get_current();
		update_edges(current);
		delete_after_redo();
		show_image();
    } else {
        QMessageBox::warning(this, "Warning", "Image not loaded");
    }
}

/*
* @brief Rotate image clockwise.
*/
void MainWindow::on_action_Rotate_right_triggered()
{
    if (m_has_image) {
		ImageParams params{m_image_list[m_image_index].m_param_list[m_image_list[m_image_index].m_index]};
		auto a{params.corners[0]}, b{params.corners[1]}, c{params.corners[2]}, d{params.corners[3]};
		params.corners[0] = c;
		params.corners[1] = a;
		params.corners[2] = d;
		params.corners[3] = b;
		m_image_list[m_image_index].m_param_list.push_back(params);
		m_image_list[m_image_index].m_index++;
		cv::Mat current = m_image_list[m_image_index].get_current();
		update_edges(current);
		delete_after_redo();
		show_image();
    } else {
        QMessageBox::warning(this, "Warning", "Image not loaded");
    }
}


/*
* @brief Convert image to RGB.
*/
void MainWindow::on_btRGB_triggered()
{
	if (m_has_image) {
		m_image_list[m_image_index].m_type = image_type::color;
		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded");
	}
}

/*
* @brief Convert image to grayscale.
*/
void MainWindow::on_btGray_triggered()
{
	if (m_has_image) {
		m_image_list[m_image_index].m_type = image_type::grayscale;
		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded");
	}
}

/*
* @brief Delete current image.
*/
void MainWindow::on_action_Delete_triggered()
{
	if (m_has_image) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "Warning", "Are you sure you want to delete current image?", QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes) {
			m_image_list.clear();
			m_slider_values.clear();
			m_image_index = m_slider_index = 0;
			m_has_image = false;
			show_image();
		}

	} else {
		QMessageBox::warning(this, "Warning", "Nothing to delete");
	}
}

// TODO: Croping
void MainWindow::on_action_Crop_triggered()
{
	if (m_has_image) {


	} else {
		QMessageBox::warning(this, "Warning", "Nothing to crop");
	}
}

/*
* @brief Resize the image to the given width and height.
*/
void MainWindow::on_action_Resize_triggered()
{
	if (m_has_image) {
		QDialog dialog(this);
		QFormLayout form(&dialog);

		form.addRow(new QLabel("Choose width and height"));

		QList<QLineEdit *> fields;

		QLineEdit *lineEdit = new QLineEdit(&dialog);
		QString label = QString("Width");
		form.addRow(label, lineEdit);
		fields << lineEdit;

		lineEdit = new QLineEdit(&dialog);
		label = QString("Height");
		form.addRow(label, lineEdit);
		fields << lineEdit;

		QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
		form.addRow(&buttonBox);

		QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
		QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

		// TODO: Check if its int, maybe leaking memory idk
		if (dialog.exec() == QDialog::Accepted) {
			cv::Mat current{m_image_list[m_image_index].get_current()};
			cv::resize(current, current, cv::Size(fields[0]->text().toInt(), fields[1]->text().toInt()));

			update_edges(current);

			Image img{current, m_image_list[m_image_index].m_filename};
			m_image_list.push_back(img);
			m_image_index++;

			delete_after_redo();
			show_image();
		}

	} else {
        QMessageBox::warning(this, "Warning", "Cannot resize image");
	}
}

/*
* @brief Exit the application.
*/
void MainWindow::on_action_Exit_triggered()
{
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, "Warning", "Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes) {
		QApplication::quit();
	}
}

/*
* @brief Undo the previous image adjustment.
*/
void MainWindow::on_action_Undo_triggered()
{
	if (m_has_image) {
		if (0 == m_image_list[m_image_index].m_index && 0 == m_image_index) {
			QMessageBox::warning(this, "Warning", "Nothing to undo");
		} else if (0 == m_image_list[m_image_index].m_index) {
			m_image_index--;
		} else {
			m_image_list[m_image_index].m_index--;
		}

		if(m_slider_index) {
			m_slider_index--;
			for(auto &&slider_pair : m_slider_values[m_slider_index]) {
				m_sliders[slider_pair.first]->setSliderPosition(slider_pair.second);
			}
		}

		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Nothing to undo");
	}
}

/*
* @brief Redo the previous image adjustment.
*/
void MainWindow::on_action_Redo_triggered()
{
	if (m_has_image) {
		if (m_image_list[m_image_index].m_index + 1 == m_image_list[m_image_index].m_param_list.size() && m_image_index + 1 == m_image_list.size()) {
			QMessageBox::warning(this, "Warning", "Nothing to redo");
		} else if (m_image_list[m_image_index].m_index + 1 == m_image_list[m_image_index].m_param_list.size()) {
			m_image_index++;
		} else {
			m_image_list[m_image_index].m_index++;
		}

		if(m_slider_index < m_slider_values.size() - 1) {
			m_slider_index++;
			for(auto &&slider_pair : m_slider_values[m_slider_index]) {
				m_sliders[slider_pair.first]->setSliderPosition(slider_pair.second);
			}
		}

		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Nothing to redo");
	}
}

/*
* @brief Delete the adjusted image after redoing adjustments.
*/
void MainWindow::delete_after_redo() {
	if (m_image_index != m_image_list.size()) {
		m_image_list.erase(m_image_list.begin() + int(m_image_index) + 1, m_image_list.end());
	}

	if (m_image_list[m_image_index].m_index != m_image_list[m_image_index].m_param_list.size()) {
		m_image_list[m_image_index].m_param_list.erase(m_image_list[m_image_index].m_param_list.begin() + int(m_image_list[m_image_index].m_index) + 1,
										   m_image_list[m_image_index].m_param_list.end());
	}

	if(m_slider_index != m_slider_values.size()) {
		m_slider_values.erase(m_slider_values.begin() + int(m_slider_index), m_slider_values.end());
	}
}

void MainWindow::on_label_clicked() {

}

void MainWindow::on_label_moved() {
	if (m_has_image) {
		auto diff = m_lb_image->get_diff();
		cv::Mat current = m_image_list[m_image_index].get_current();
		if (m_current_left - diff.first >= 0 && m_current_right - diff.first <= current.cols) {
			m_current_left -= diff.first;
			m_current_right -= diff.first;
		}
		if (m_current_top - diff.second >= 0 && m_current_bottom - diff.second <= current.rows) {
			m_current_top -= diff.second;
			m_current_bottom -= diff.second;
		}
		show_image();
	}
}

void MainWindow::on_label_released() {

}

void MainWindow::update_edges(const cv::Mat& current) {
	if (current.cols > 600) {
		m_current_left = (current.cols - 600)/2;
		m_current_right = current.cols - (current.cols - 600)/2;
	}
	else {
		m_current_left = 0;
		m_current_right = current.cols;
	}
	if (current.rows > 600) {
		m_current_top = (current.rows - 600)/2;
		m_current_bottom = current.rows - (current.rows - 600)/2;
	}
	else {
		m_current_top = 0;
		m_current_bottom = current.rows;
	}
}
