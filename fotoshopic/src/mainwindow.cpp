#include "headers/mainwindow.h"
#include "ui_mainwindow.h"

#include "headers/section.h"

#include <QButtonGroup>
#include <QRadioButton>

#include <headers/edit_operations.h>
#include <headers/image_operations.h>


MainWindow::MainWindow(QWidget *parent)
	:	QMainWindow(parent),
		m_lb_image{new QLabel},
		ui{new Ui::MainWindow},
		m_has_image{false}
{
	ui->setupUi(this);

    // Disable spacing
    // TODO: Pls lice radi nesto
    auto *hlMain{new QHBoxLayout};
	ui->mainContainer->setLayout(hlMain);
	ui->mainContainer->setMaximumWidth(700);
    hlMain->setStretchFactor(hlMain, 30);

	// Set side pannel alignment
    // TODO: Add slots and signals for sliders
	ui->hlSide->setAlignment(Qt::AlignTop);

	// Create basic photo adjustment sliders
    auto basic_sliders{create_section("Basic settings", {"Brightness", "Contrast", "Shadows", "Highlights", "Whites", "Blacks"})};

	QObject::connect(basic_sliders[0], &QSlider::sliderMoved, [&](auto &&e) {
		if(m_has_image) {
			push_operation(new fs::ops::BasicEditOperation(m["brightness"], e, fs::ops::basic_edits::brightness));
			m["brightness"] = e;
			show_image();
		} else {
			QMessageBox::warning(this, "Warning", "No image to adjust.");
		}
	});

	QObject::connect(basic_sliders[1], &QSlider::sliderMoved, [&](auto &&e) {
		if(m_has_image) {
			push_operation(new fs::ops::BasicEditOperation(m["contrast"], e, fs::ops::basic_edits::contrast));
			m["contrast"] = e;
			show_image();
		} else {
			QMessageBox::warning(this, "Warning", "No image to adjust.");
		}
	});

	// Create advanced photo adjustment sliders
	auto advanced_sliders{create_section("Advanced settings", {"Sharpen", "Vignette", "Blur"})};
	advanced_sliders.back()->setSliderPosition(0);
	advanced_sliders.front()->setSliderPosition(0);

	// Create color photo adjustment sliders
    auto color_sliders{create_section("Color settings", {"Saturation", "Luminance", "Temperature"})};

	QObject::connect(color_sliders[0], &QSlider::sliderMoved, [&](auto &&e) {
		if(m_has_image) {
			push_operation(new fs::ops::BasicEditOperation(m["saturation"], e, fs::ops::basic_edits::saturation));
			m["saturation"] = e;
			show_image();
		} else {
			QMessageBox::warning(this, "Warning", "No image to adjust.");
		}
	});

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
			if(clicked) {
				for(auto *e : m_sections)
				{
					if(*e != *section) {
						e->colapse();
					}
				}
			}

			if(clicked) {
				section->expand();
			} else {
				section->colapse();
			}
		});
	}
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::show_image() const
{
	if (m_has_image) {
		cv::Mat tmp;

		// Show the image either as a colored image or a grayscale image
		if (0 == img.m_type) {
			cv::cvtColor(img.m_img, tmp, cv::COLOR_BGR2GRAY);
			m_lb_image->setPixmap(QPixmap::fromImage(QImage(tmp.data, tmp.cols, tmp.rows, int(tmp.step), QImage::Format_Indexed8)));
		} else if (1 == img.m_type) {
			cv::cvtColor(img.m_img, tmp, cv::COLOR_BGR2RGB);
			m_lb_image->setPixmap(QPixmap::fromImage(QImage(tmp.data, tmp.cols, tmp.rows, int(tmp.step), QImage::Format_RGB888)));
		}
	} else {
		m_lb_image->clear();
	}
}

void MainWindow::save_image(const std::string& fileName)
{
	try {
		cv::Mat tmp;

		// Save image as either a colored image or a grayscale image
		if (0 == img.m_type)
			cv::cvtColor(tmp, tmp, cv::COLOR_BGR2GRAY);
		else if (1 == img.m_type)
			cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGB);

		cv::imwrite(fileName, tmp);
	} catch (...) {
		QMessageBox::warning(this, "Warning", "Cannot save file" + QString::fromStdString(fileName));
	}
}

void MainWindow::push_operation(fs::ops::AbstractOperation *op)
{
    op->apply(img);
    m_fwd_ops.emplace_back(std::move(op));
}

void MainWindow::pop_operation()
{
    auto op{std::move(m_fwd_ops.back())};
    m_fwd_ops.pop_back();
    op->invert(img);
    m_bwd_ops.emplace_back(std::move(op));
}

void MainWindow::on_action_Open_triggered()
{
    if(m_has_image) {
        auto reply{QMessageBox::question(this, "Warning", "All unsaved changes will be lost. Do you wish to proceed?", QMessageBox::Yes | QMessageBox::No)};
        if(QMessageBox::No == reply) {
            return;
        }
    }

    // NOTE: Emits a warning.
    QString fileName{QFileDialog::getOpenFileName(this, "Open file")};
	try {
        cv::Mat tmp{cv::imread(fileName.toStdString())};
        img = Image(tmp, fileName.toStdString());
        m_has_image = true;
		show_image();
		setWindowTitle(fileName);
	} catch (...) {
		QMessageBox::warning(this, "Warning", "Cannot open file" + fileName);
	}
}

std::vector<QSlider*> MainWindow::create_section(QString name, const std::vector<QString> &contents)
{
    Section *section{new Section(name, 300, this)};
	m_sections.push_back(section);
	ui->hlSide->addWidget(section);

    auto *vbox{new QVBoxLayout()};
    std::vector<QSlider*> sliders;

	for(auto &&e : contents)
	{
        vbox->addWidget(new QLabel(e, section));
        sliders.push_back(new QSlider(Qt::Horizontal, section));
        vbox->addWidget(sliders.back());
	}

	for (auto &&e : sliders) {
        e->setSliderPosition(50);
    }

    section->setContentLayout(*vbox);
    return sliders;
}

std::pair<std::vector<QSlider*>, QButtonGroup*> MainWindow::create_section(QString name, const std::vector<QString> &contents, int buttons)
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
    std::vector<QSlider*> sliders;
    vbox->addWidget(rbuttons);

    for(auto &&e : contents)
    {
        vbox->addWidget(new QLabel(e, section));
        sliders.push_back(new QSlider(Qt::Horizontal, section));
        vbox->addWidget(sliders.back());
    }

	for (auto &&e : sliders) {
        e->setSliderPosition(50);
    }

	if(buttons) {
		toggle_group->buttons()[0]->setChecked(true);
	}

    section->setContentLayout(*vbox);
    return {sliders, toggle_group};
}

void MainWindow::on_action_Save_triggered()
{
	if (m_has_image) {
		save_image(img.m_filename);
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded");
	}
}

void MainWindow::on_action_SaveAs_triggered()
{
	if (m_has_image) {
		QString fileName = QFileDialog::getSaveFileName(this, "Save as...");
		save_image(fileName.toStdString());
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded");
	}
}

// TODO: Fix zooming, implement dragging while zoomed [@milanilic332]
// TODO: Check if image is to large or small
void MainWindow::on_action_ZoomIn_triggered()
{
	if (m_has_image) {
		push_operation(new fs::ops::ResizeOperation(int(img.m_img.cols*1.1), int(img.m_img.rows*1.1)));
		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded");
	}
}

// TODO: Fix zooming, implement dragging while zoomed [@milanilic332]
void MainWindow::on_action_ZoomOut_triggered()
{
	if (m_has_image) {
		push_operation(new fs::ops::ResizeOperation(int(img.m_img.cols*0.9), int(img.m_img.rows*0.9)));
		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded");
	}
}

void MainWindow::on_action_Mirror_triggered()
{
	if (m_has_image) {
        push_operation(new fs::ops::MirrorOperation);
		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded");
	}
}

void MainWindow::on_action_Rotate_left_triggered()
{
    if (m_has_image) {
        push_operation(new fs::ops::RotateLeftOperation);
        show_image();
    } else {
        QMessageBox::warning(this, "Warning", "Image not loaded");
    }
}

void MainWindow::on_action_Rotate_right_triggered()
{
    if (m_has_image) {
        push_operation(new fs::ops::RotateRightOperation);
        show_image();
    } else {
        QMessageBox::warning(this, "Warning", "Image not loaded");
    }
}


void MainWindow::on_btRGB_triggered()
{
	if (m_has_image) {
		img.m_type = 1;
		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded");
	}
}

void MainWindow::on_btGray_triggered()
{
	if (m_has_image) {
		img.m_type = 0;
		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded");
	}
}

void MainWindow::on_action_Delete_triggered()
{
	if (m_has_image) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "Warning", "Are you sure you want to delete current image?",
									  QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes) {
			img = {};
			m_has_image = false;
			show_image();
		}

	} else {
		QMessageBox::warning(this, "Warning", "Nothing to delete");
	}
}

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
            push_operation(new fs::ops::ResizeOperation(fields[0]->text().toInt(), fields[1]->text().toInt()));
			show_image();
		}

	} else {
        QMessageBox::warning(this, "Warning", "Cannot resize image");
	}
}

void MainWindow::on_action_Exit_triggered()
{
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, "Warning", "Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes) {
		QApplication::quit();
	}
}

void MainWindow::on_action_Undo_triggered()
{
    if(m_fwd_ops.empty()) {
        QMessageBox::information(this, "Info", "Nothing to undo.");
        return;
    }

    pop_operation();
    show_image();
}

void MainWindow::on_action_Redo_triggered()
{
    if(m_bwd_ops.empty()) {
        QMessageBox::information(this, "Info", "Nothing to redo.");
        return;
    }

    auto op{std::move(m_bwd_ops.back())};
    m_bwd_ops.pop_back();
    push_operation(op.release());
    show_image();
}
