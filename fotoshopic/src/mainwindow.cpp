#include "headers/mainwindow.h"
#include "ui_mainwindow.h"
#include "headers/utils.h"
#include "headers/section.h"


/*
* @brief Builds instance of MainWindow.
*/
MainWindow::MainWindow(QWidget *parent)
	:	QMainWindow(parent),
		m_lb_image{new QLabel},
		ui{new Ui::MainWindow},
		m_has_image{false}
{
	ui->setupUi(this);

    // Disable spacing
	// TODO: Adjust sidebar and image label size
    auto *hlMain{new QHBoxLayout};
	ui->mainContainer->setLayout(hlMain);
	ui->mainContainer->setMaximumWidth(700);
    hlMain->setStretchFactor(hlMain, 30);

	// Set side pannel alignment
    // TODO: Add slots and signals for sliders
	ui->hlSide->setAlignment(Qt::AlignTop);

	// Create basic photo adjustment sliders
	auto basic_sliders{create_section("Basic settings", {"Brightness", "Contrast", "Shadows", "Highlights"})};
	slider_operation(basic_sliders, "Brightness");
	slider_operation(basic_sliders, "Contrast");
	slider_operation(basic_sliders, "Shadows");
	slider_operation(basic_sliders, "Highlights");

	// Create advanced photo adjustment sliders
	auto advanced_sliders{create_section("Advanced settings", {"Sharpen", "Vignette", "Blur"})};
	slider_operation(advanced_sliders, "Sharpen", 0);
	slider_operation(advanced_sliders, "Vignette", 0);
	slider_operation(advanced_sliders, "Blur", 0);

	// Create color photo adjustment sliders
    auto color_sliders{create_section("Color settings", {"Saturation", "Luminance", "Temperature"})};
	slider_operation(color_sliders, "Saturation");
	slider_operation(color_sliders, "Luminance");
	slider_operation(color_sliders, "Temperature");

    // TODO: Add color selection
	// Create individual color photo adjustment sliders
    auto color_individual_sliders{create_section("Individual color settings", {"Hue", "Saturation", "Luminance"}, 3)};

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
		cv::Mat current{image_list[index].get_current()};
		if (0 == image_list[index].m_type) {
			cv::cvtColor(current, current, cv::COLOR_BGR2GRAY);
			m_lb_image->setPixmap(QPixmap::fromImage(QImage(current.data, current.cols, current.rows, int(current.step), QImage::Format_Indexed8)));
		} else if (1 == image_list[index].m_type) {
			cv::cvtColor(current, current, cv::COLOR_BGR2RGB);
			m_lb_image->setPixmap(QPixmap::fromImage(QImage(current.data, current.cols, current.rows, int(current.step), QImage::Format_RGB888)));
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
		cv::Mat current{image_list[index].get_current()};
		if (0 == img.m_type) {
			cv::cvtColor(current, current, cv::COLOR_BGR2GRAY);
		} else if (1 == img.m_type) {
			cv::cvtColor(current, current, cv::COLOR_BGR2RGB);
		}

		cv::imwrite(fileName, current);
	} catch (...) {
		QMessageBox::warning(this, "Warning", "Cannot save file" + QString::fromStdString(fileName));
	}
}

/*
* @brief Set initial slider value and define slider movement trigger.
*/
void MainWindow::slider_operation(qstring_map<QSlider*> sliders, const QString &key, int value)
{

	sliders[key]->setSliderPosition(value);

	QObject::connect(sliders[key], &QSlider::sliderMoved, [this, key](auto &&e) {
		if(m_has_image) {
			if(index) {
				image_list[index].param_list[image_list[index].index].m_adjustment_map[key].first = image_list[index-1].param_list[image_list[index-1].index].m_adjustment_map[key].second;
			} else {
				image_list[index].param_list[image_list[index].index].m_adjustment_map[key].first = image_list[index].param_list[image_list[index].index].m_adjustment_map[key].second;
			}

			image_list[index].param_list[image_list[index].index].m_adjustment_map[key].second = e;

			cv::Mat current{image_list[index].get_current()};
			Image img{current, image_list[index].m_filename};
			image_list.push_back(img);
			index++;
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

	image_list.clear();

    QString fileName{QFileDialog::getOpenFileName(this, "Open file")};
	try {
		Image img{cv::imread(fileName.toStdString()), fileName.toStdString()};
		image_list.push_back(img);
		index = 0;
        m_has_image = true;
		show_image();
		setWindowTitle(fileName);
	} catch (...) {
		QMessageBox::warning(this, "Warning", "Cannot open file" + fileName);
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
std::pair<qstring_map<QSlider*>, QButtonGroup*> MainWindow::create_section(QString name, const std::vector<QString> &contents, int buttons)
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

	if(buttons) {
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
		save_image(image_list[index].m_filename);
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
		cv::Mat current{image_list[index].get_current()};
		cv::resize(current, current, cv::Size(), 1.1, 1.1);
		Image img{current, image_list[index].m_filename};
		image_list.push_back(img);
		index++;
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
		cv::Mat current{image_list[index].get_current()};
		cv::resize(current, current, cv::Size(), 0.9, 0.9);
		Image img{current, image_list[index].m_filename};
		image_list.push_back(img);
		index++;
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
		image_params params{image_list[index].param_list[image_list[index].index]};
		std::swap(params.topleft_corner, params.topright_corner);
		std::swap(params.bottomleft_corner, params.bottomright_corner);
		image_list[index].param_list.push_back(params);
		image_list[index].index++;
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
		image_params params{image_list[index].param_list[image_list[index].index]};
		int a{params.topleft_corner}, b{params.topright_corner}, c{params.bottomleft_corner}, d{params.bottomright_corner};
		params.topleft_corner = b;
		params.topright_corner = d;
		params.bottomleft_corner = a;
		params.bottomright_corner = c;
		image_list[index].param_list.push_back(params);
		image_list[index].index++;
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
		image_params params{image_list[index].param_list[image_list[index].index]};
		int a{params.topleft_corner}, b{params.topright_corner}, c{params.bottomleft_corner}, d{params.bottomright_corner};
		params.topleft_corner = c;
		params.topright_corner = a;
		params.bottomleft_corner = d;
		params.bottomright_corner = b;
		image_list[index].param_list.push_back(params);
		image_list[index].index++;
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
		image_list[index].m_type = 1;
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
		image_list[index].m_type = 0;
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
			image_list.clear();
			index = 0;
			m_has_image = false;
			show_image();
		}

	} else {
		QMessageBox::warning(this, "Warning", "Nothing to delete");
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
			cv::Mat current{image_list[index].get_current()};
			cv::resize(current, current, cv::Size(fields[0]->text().toInt(), fields[1]->text().toInt()));
			Image img{current, image_list[index].m_filename};
			image_list.push_back(img);
			index++;
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
		if (0 == image_list[index].index && 0 == index) {
			QMessageBox::warning(this, "Warning", "Nothing to undo");
		} else if (0 == image_list[index].index) {
			index--;
		} else {
			image_list[index].index--;
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
		if (image_list[index].index + 1 == image_list[index].param_list.size() && index + 1 == image_list.size()) {
			QMessageBox::warning(this, "Warning", "Nothing to redo");
		} else if (image_list[index].index + 1 == image_list[index].param_list.size()) {
			index++;
		} else {
			image_list[index].index++;
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
	if (image_list.begin() + int(index) != image_list.end()) {
		image_list.erase(image_list.begin() + int(index) + 1, image_list.end());
	}

	if (image_list[index].param_list.begin() + int(image_list[index].index) != image_list[index].param_list.end()) {
		image_list[index].param_list.erase(image_list[index].param_list.begin() + int(image_list[index].index) + 1,
										   image_list[index].param_list.end());
	}
}
