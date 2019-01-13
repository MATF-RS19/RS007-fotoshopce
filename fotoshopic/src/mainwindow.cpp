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
		m_lb_image{new MouseLabel},
		m_has_image{false}
{
	ui->setupUi(this);

	// Connects mouse move event on label to slot here
	// TODO: New system
	connect(m_lb_image, SIGNAL(moved()), this, SLOT(on_label_moved()));

	auto *main_layout{new QHBoxLayout};
	ui->mainContainer->setLayout(main_layout);

	// Set side panel alignment
	ui->hlSide->setAlignment(Qt::AlignTop);

	// Create photo adjustment sliders
	create_section("Basic settings", {{"Brightness", 50}, {"Contrast", 50}, {"Saturation", 50}});
	create_section("Advanced settings", {{"Sharpen", 0}, {"Vignette", 0}, {"Blur", 0}});
	create_section("Color settings", {{"Hue", 50}, {"Saturation", 50}, {"Value", 50}});
	create_filter_section("Filters");
	create_type_section("Color mode");
	sync_sections();

	// Setting Widget params
	m_lb_image->setMinimumSize(ui->centralWidget->width(), ui->centralWidget->height());

	// Future use for cropping
	m_lb_image->setMouseTracking(true);
	m_lb_image->setStyleSheet("QLabel { background-color : #191919; }");
	m_lb_image->setAlignment(Qt::AlignCenter);

	// Add image to main widget
	main_layout->addWidget(m_lb_image);
}

/*
* @brief Destroys MainWindow.
*/
MainWindow::~MainWindow()
{
	delete ui;
}

/*
* @brief Updates parameters when window is resized.
*/
void MainWindow::resizeEvent(QResizeEvent*) {
	if (m_has_image) {
		// Milane jebem ti mater nauci da ne koristis = za inicijalizaciju.
		auto params{update_edges_and_size()};
		auto img{m_history.current_template()};
		m_history.add_entry(img, params);
		show_image();
	}
}

/*
* @brief Display image according to its type (either RGB or grayscale).
*/
void MainWindow::show_image()
{
	if (m_has_image) {
		// Takodje jebo te camel case
		auto img{m_history.current_image()};
		auto params{m_history.current_parameters()};
		cv::Rect myROI(params.current_left,
					   params.current_top,
					   params.current_right - params.current_left,
					   params.current_bottom - params.current_top);
		cv::Mat cropped{img.m_img(myROI)};
		cv::cvtColor(cropped, cropped, cv::COLOR_BGR2RGB);
		m_lb_image->setPixmap(QPixmap::fromImage(QImage(cropped.data, cropped.cols, cropped.rows, int(cropped.step), QImage::Format_RGB888)));
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
		auto img{m_history.current_image()};
		cv::imwrite(filename, img.m_img);
	} catch (...) {
		QMessageBox::warning(this, "Warning", "Cannot save file" + QString::fromStdString(filename));
	}
}

/*
* @brief Make only one section open at any given time.
*/
void MainWindow::sync_sections()
{
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
* @brief Set initial slider value and define slider movement trigger.
*/
void MainWindow::slider_operation(QSlider *slider, const QString &name, int value)
{
	slider->setSliderPosition(value);
	slider->setTracking(false);

	QObject::connect(slider, &QSlider::valueChanged, [name, slider, value, this](auto &&e) {
		if(m_has_image) {
			auto img{m_history.current_template()};
			auto parameters{m_history.current_parameters()};
			parameters.adjustment_map[name] = e;
			m_history.add_entry(img, parameters);
			show_image();
		} else {
			slider->setSliderPosition(value);
			QMessageBox::warning(this, "Warning", "No image to adjust.");
		}
	});
}

/*
* @brief Create one drop-down slider section.
*/
void MainWindow::create_section(const QString &name, const std::vector<std::pair<QString, int>> &contents)
{
	auto *section{new Section(name, 300, this)};
	m_sections.push_back(section);
	ui->hlSide->addWidget(section);

	auto *vbox{new QVBoxLayout()};

	for(auto &&e : contents)
	{
		vbox->addWidget(new QLabel(e.first, section));
		m_sliders[e.first] = new QSlider(Qt::Horizontal, section);
		slider_operation(m_sliders[e.first], e.first, e.second);
		vbox->addWidget(m_sliders[e.first]);
	}

	section->setContentLayout(*vbox);
}

/*
* @brief Read file names for filter image icons.
*/
std::vector<std::pair<QString, QString>> MainWindow::read_filter_filenames()
{
	std::vector<std::pair<QString, QString>> filter_filenames;
	QFile infile(QString(":icons/filter_paths.txt"));
	infile.open(QIODevice::ReadOnly);
	if(!infile.isOpen()) {
		QMessageBox::warning(this, "Warning", "Cannot open filter paths file.");
		return filter_filenames;
	}

	QTextStream stream(&infile);
	QString line{stream.readLine()};

	while(!line.isNull()) {
		auto split_line{line.split(QString(","))};
		filter_filenames.push_back({split_line[0].trimmed(), split_line[1].trimmed()});
		line = stream.readLine();
	}

	return filter_filenames;
}

/*
* @brief Create drop-down filter section.
*/
void MainWindow::create_filter_section(const QString &name)
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

	for(auto &&row : filters)
	{
		auto *widget{new QWidget(this)};
		auto *hbox{new QHBoxLayout};
		hbox->setAlignment(Qt::AlignCenter);
		widget->setLayout(hbox);
		vbox->addWidget(widget);

		for(auto &&col : row)
		{
			auto *button{new QPushButton};
			hbox->addWidget(button);
			filter_buttons.push_back({button, col});
			auto b_filter{col};
			button->setCheckable(true);
			button->setChecked(false);

			QObject::connect(button, &QPushButton::clicked, [button, b_filter, filter_buttons, this](auto &&e) {
				if(m_has_image) {
					for(auto &&e : filter_buttons) {
						if(e.first->isChecked() && e.first != button) {
							e.first->setChecked(false);
						}
					}

					auto img{m_history.current_template()};
					auto params{m_history.current_parameters()};
					params.filter = e ? b_filter : filters::none;
					m_history.add_entry(img, params);
					show_image();
				} else {
					QMessageBox::warning(this, "Warning", "No image to adjust.");
				}
			});
		}
	}

	auto filter_filenames{read_filter_filenames()};
	for(size_t i = 0; i < filter_buttons.size(); ++i)
	{
		filter_buttons[i].first->setIcon(QIcon(filter_filenames[i].first));
		filter_buttons[i].first->setIconSize(QSize(69, 69));
		filter_buttons[i].first->setToolTip(filter_filenames[i].second);
	}

	section->setContentLayout(*vbox);
}

/*
 * @brief Create drop-down image type section.
 */
void MainWindow::create_type_section(const QString &name)
{
	Section *section{new Section(name, 300, this)};
	m_sections.push_back(section);
	ui->hlSide->addWidget(section);

	auto *vbox{new QVBoxLayout};
	vbox->setAlignment(Qt::AlignTop);

	std::vector<std::pair<QPushButton*, image_type>> image_type_buttons;
	std::vector<std::pair<image_type, QString>> image_types{{image_type::grayscale, QString::fromStdString("Grayscale")},
															{image_type::color, QString::fromStdString("Color")}};

	for(auto &&img_type : image_types)
	{
		auto *button{new QPushButton};
		vbox->addWidget(button);
		image_type_buttons.push_back({button, img_type.first});
		auto b_type{img_type.first};
		button->setCheckable(true);
		button->setChecked(false);
		button->setToolTip(img_type.second);
		button->setText(img_type.second);

		QObject::connect(button, &QPushButton::clicked, [b_type, image_type_buttons, button, this](auto &&e) {
			if(m_has_image) {
				for(auto &&e : image_type_buttons) {
					if(e.first->isChecked() && e.first != button) {
						e.first->setChecked(false);
					}
				}

				auto img{m_history.current_template()};
				auto params{m_history.current_parameters()};
				params.img_type = e ? b_type : image_type::color;
				m_history.add_entry(img, params);
				show_image();
			} else {
				QMessageBox::warning(this, "Warning", "No image to adjust.");
			}
		});
	}

	section->setContentLayout(*vbox);
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

	QString filename{QFileDialog::getOpenFileName(this, "Open file.")};
	m_filename = filename.toStdString();

	try {
		image img(cv::imread(filename.toStdString()));
        m_has_image = true;

		for(auto &&e : image_parameters().adjustment_map) {
			m_sliders[e.first]->setSliderPosition(e.second);
		}

		auto current_width{m_lb_image->size().width()};
		auto current_height{m_lb_image->size().height()};
		image_parameters params;

		params.size = {img.m_img.cols, img.m_img.rows};

		if (img.m_img.cols > current_width) {
			params.current_left = (img.m_img.cols - current_width)/2;
			params.current_right = img.m_img.cols - (img.m_img.cols - current_width)/2;
		} else {
			params.current_left = 0;
			params.current_right = img.m_img.cols;
		}

		if (img.m_img.rows > current_height) {
			params.current_top = (img.m_img.rows - current_height)/2;
			params.current_bottom = img.m_img.rows - (img.m_img.rows - current_height)/2;
		} else {
			params.current_top = 0;
			params.current_bottom = img.m_img.rows;
		}

		m_history.set_initial(img, params);
		ui->statusBar->showMessage(filename);
		show_image();
	} catch (...) {
		QMessageBox::warning(this, "Warning", "Cannot open file." + filename);
	}
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
		m_filename = filename.toStdString();
		ui->statusBar->showMessage(filename);
		save_image(m_filename);
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded.");
	}
}

/*
* @brief Zoom in image
*/
void MainWindow::on_action_ZoomIn_triggered()
{
	if (m_has_image) {
		auto img{m_history.current_template()};
		auto params{m_history.current_parameters()};
		params.size.first = int(params.size.first * 1.1);
		params.size.second = int(params.size.second * 1.1);
		m_history.add_entry(img, params);
		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded.");
	}
}

/*
* @brief Zoom out image
*/
void MainWindow::on_action_ZoomOut_triggered()
{
	if (m_has_image) {
		auto img{m_history.current_template()};
		auto params{m_history.current_parameters()};
		params.size.first = int(params.size.first * 0.9);
		params.size.second = int(params.size.second * 0.9);
		m_history.add_entry(img, params);
		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded.");
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
* @brief Delete current image.
*/
// TODO: Make this undoable
void MainWindow::on_action_Delete_triggered()
{
	if (m_has_image) {
		QMessageBox::StandardButton reply{QMessageBox::question(this, "Warning", "Are you sure you want to delete current image?", QMessageBox::Yes | QMessageBox::No)};
		if (reply == QMessageBox::Yes) {
			m_has_image = false;
			show_image();
		}
	} else {
		QMessageBox::warning(this, "Warning", "Nothing to delete");
	}
}

// TODO: Cropping
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
		form.addRow(new QLabel("Choose width and height."));

		QList<QLineEdit*> fields;

		QLineEdit *line_edit{new QLineEdit(&dialog)};
		QString label{"Width"};
		form.addRow(label, line_edit);
		fields << line_edit;

		line_edit = new QLineEdit(&dialog);
		label = "Height";
		form.addRow(label, line_edit);
		fields << line_edit;

		QDialogButtonBox button_box(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
		form.addRow(&button_box);

		QObject::connect(&button_box, SIGNAL(accepted()), &dialog, SLOT(accept()));
		QObject::connect(&button_box, SIGNAL(rejected()), &dialog, SLOT(reject()));

		// TODO: Check if its int, maybe leaking memory idk
		if (dialog.exec() == QDialog::Accepted) {
			image img{m_history.current_template()};
			cv::resize(img.m_img, img.m_img, cv::Size(fields[0]->text().toInt(), fields[1]->text().toInt()));
			auto params{update_edges_and_size()};
			m_history.add_entry(img, params);
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
		auto values{m_history.undo().second};
		for(auto &&e : values.adjustment_map) {
			m_sliders[e.first]->setSliderPosition(e.second);
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
		auto values{m_history.redo().second};
		for(auto &&e : values.adjustment_map) {
			m_sliders[e.first]->setSliderPosition(e.second);
		}

		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Nothing to redo");
	}
}

/*
* @brief Enables dragging image when its bigger than label
*/
void MainWindow::on_label_moved()
{
	if (m_has_image) {
		auto diff{m_lb_image->get_diff()};
		auto img{m_history.current_image()};
		auto params{m_history.current_parameters()};

		if (params.current_left - diff.first >= 0 &&
			params.current_right - diff.first <= img.m_img.cols) {
			params.current_left -= diff.first;
			params.current_right -= diff.first;
		}
		if (params.current_top - diff.second >= 0 &&
			params.current_bottom - diff.second <= img.m_img.rows) {
			params.current_top -= diff.second;
			params.current_bottom -= diff.second;
		}

		m_history.add_entry(img, params);
		show_image();
	}
}

/*
* @brief Updates image size and current edges.
*/
image_parameters MainWindow::update_edges_and_size() const
{
	auto params{m_history.current_parameters()};
	auto img{m_history.current_template()};
	auto current_width{m_lb_image->size().width()};
	auto current_height{m_lb_image->size().height()};

	params.size = {img.m_img.cols, img.m_img.rows};

	if (img.m_img.cols > current_width) {
		params.current_left = (img.m_img.cols - current_width)/2;
		params.current_right = img.m_img.cols - (img.m_img.cols - current_width)/2;
	} else {
		params.current_left = 0;
		params.current_right = img.m_img.cols;
	}

	if (img.m_img.rows > current_height) {
		params.current_top = (img.m_img.rows - current_height)/2;
		params.current_bottom = img.m_img.rows - (img.m_img.rows - current_height)/2;
	} else {
		params.current_top = 0;
		params.current_bottom = img.m_img.rows;
	}

	return params;
}
