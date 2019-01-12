#include <iostream>

#include "ui_mainwindow.h"
#include "headers/mainwindow.h"
#include "headers/utils.h"
#include "headers/image.h"
#include "headers/section.h"


/*
* @brief Builds instance of MainWindow.
*/
MainWindow::MainWindow(QWidget *parent)
	:	QMainWindow(parent),
		ui{new Ui::MainWindow},
		m_lb_image{new QLabel},
		m_has_image{false},
		m_slider_index{0},
		m_slider_values(1)
{
	ui->setupUi(this);

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
	auto filter_section{create_section("Filters", {}, 4, false)};

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
		auto img{m_history.current_image()};

		if (image_type::grayscale == img.m_type) {
			cv::cvtColor(img.m_img, img.m_img, cv::COLOR_BGR2GRAY);
			m_lb_image->setPixmap(QPixmap::fromImage(QImage(img.m_img.data, img.m_img.cols, img.m_img.rows, int(img.m_img.step), QImage::Format_Indexed8)));
		} else if (image_type::color == img.m_type) {
			cv::cvtColor(img.m_img, img.m_img, cv::COLOR_BGR2RGB);
			m_lb_image->setPixmap(QPixmap::fromImage(QImage(img.m_img.data, img.m_img.cols, img.m_img.rows, int(img.m_img.step), QImage::Format_RGB888)));
		}
	} else {
		m_lb_image->clear();
	}
}

/*
* @brief Save image according to its type (either RGB or grayscale).
*/
void MainWindow::save_image(const std::string& fileName)
{
	try {
		auto img{m_history.current_image()};

		if (image_type::grayscale == img.m_type) {
			cv::cvtColor(img.m_img, img.m_img, cv::COLOR_BGR2GRAY);
		} else if (image_type::color == img.m_type) {
			cv::cvtColor(img.m_img, img.m_img, cv::COLOR_BGR2RGB);
		}

		cv::imwrite(fileName, img.m_img);
	} catch (...) {
		QMessageBox::warning(this, "Warning", "Cannot save file" + QString::fromStdString(fileName));
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
			auto img{m_history.current_template()};
			auto parameters{m_history.current_parameters()};
			parameters.adjustment_map[key] = e;
			m_history.add_entry(img, parameters);

			m_slider_values.push_back(parameters.adjustment_map);
			m_slider_index++;

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

	QString filename{QFileDialog::getOpenFileName(this, "Open file")};
	m_filename = filename.toStdString();

	try {
		m_slider_values.clear();
		m_slider_index = 0;

		image img(cv::imread(filename.toStdString()));
		m_history.set_initial(img);
        m_has_image = true;

		setWindowTitle(filename);
		show_image();
	} catch (...) {
		QMessageBox::warning(this, "Warning", "Cannot open file" + filename);
	}
}

/*
* @brief Create one drop-down slider section.
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
* @brief Create one drop-down section with individual color selection.
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
		save_image(m_filename);
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded.");
	}
}

/*
* @brief Save and name current image.
*/
void MainWindow::on_action_SaveAs_triggered()
{
	if (m_has_image) {
		QString filename{QFileDialog::getSaveFileName(this, "Save as...")};
		save_image(filename.toStdString());
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded.");
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
//		cv::Mat current{m_image_list[m_image_index].get_current()};
//		cv::resize(current, current, cv::Size(), 1.1, 1.1);
//		image img{current, m_image_list[m_image_index].m_filename};
//		m_image_list.push_back(img);
//		m_image_index++;
//		delete_after_redo();
//		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded.");
	}
}

// TODO: Fix zooming, implement dragging while zoomed [@milanilic332]
/*
* @brief TODO: description
*/
void MainWindow::on_action_ZoomOut_triggered()
{
	if (m_has_image) {
//		cv::Mat current{m_image_list[m_image_index].get_current()};
//		cv::resize(current, current, cv::Size(), 0.9, 0.9);
//		image img{current, m_image_list[m_image_index].m_filename};
//		m_image_list.push_back(img);
//		m_image_index++;
//		delete_after_redo();
//		show_image();
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
		image img{m_history.current_template()};
		cv::flip(img.m_img, img.m_img, 1);
		m_history.add_entry(img, m_history.current_parameters());
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
		image img(m_history.current_template());
		cv::rotate(img.m_img, img.m_img, cv::ROTATE_90_COUNTERCLOCKWISE);
		m_history.add_entry(img, m_history.current_parameters());
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
		image img(m_history.current_template());
		cv::rotate(img.m_img, img.m_img, cv::ROTATE_90_CLOCKWISE);
		m_history.add_entry(img, m_history.current_parameters());
		show_image();
    } else {
        QMessageBox::warning(this, "Warning", "Image not loaded");
    }
}


/*
* @brief Convert image to RGB.
*/
void MainWindow::on_btRGB_triggered() {}

/*
* @brief Convert image to grayscale.
*/
void MainWindow::on_btGray_triggered() {}

/*
* @brief Delete current image.
*/
void MainWindow::on_action_Delete_triggered()
{
	if (m_has_image) {
		QMessageBox::StandardButton reply{QMessageBox::question(this, "Warning", "Are you sure you want to delete current image?", QMessageBox::Yes | QMessageBox::No)};
		if (reply == QMessageBox::Yes) {
			m_slider_values.clear();
			m_slider_index = 0;
			m_has_image = false;
			show_image();
		}
	} else {
		QMessageBox::warning(this, "Warning", "Nothing to delete");
	}
}

void MainWindow::on_action_Crop_triggered()
{
//	if (m_has_image) {
//		QDialog dialog(this);
//		QFormLayout form(&dialog);
//		QLabel screen{""};
//		form.addRow(&screen);

//		cv::Mat current{m_image_list[m_image_index].get_current()};
//		if (0 == m_image_list[m_image_index].m_type) {
//			cv::cvtColor(current, current, cv::COLOR_BGR2GRAY);
//			screen.setPixmap(QPixmap::fromImage(QImage(current.data, current.cols, current.rows, int(current.step), QImage::Format_Indexed8)));
//		} else if (1 == m_image_list[m_image_index].m_type) {
//			cv::cvtColor(current, current, cv::COLOR_BGR2RGB);
//			screen.setPixmap(QPixmap::fromImage(QImage(current.data, current.cols, current.rows, int(current.step), QImage::Format_RGB888)));
//		}

//		QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
//		form.addRow(&buttonBox);

//		QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
//		QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

//		// TODO: Check if its int, maybe leaking memory idk
//		if (dialog.exec() == QDialog::Accepted) {
//			Image img{current, m_image_list[m_image_index].m_filename};
//			m_image_list.push_back(img);
//			m_image_index++;
//			delete_after_redo();
//			show_image();
//		}

//	} else {
//		QMessageBox::warning(this, "Warning", "Nothing to crop");
//	}
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

		QList<QLineEdit*> fields;

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
			image img{m_history.current_template()};
			cv::resize(img.m_img, img.m_img, cv::Size(fields[0]->text().toInt(), fields[1]->text().toInt()));
			m_history.add_entry(img, m_history.current_parameters());
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
	if (m_has_image && m_history.undoable()) {
		m_history.undo();

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
	if (m_has_image && m_history.redoable()) {
		m_history.redo();

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

