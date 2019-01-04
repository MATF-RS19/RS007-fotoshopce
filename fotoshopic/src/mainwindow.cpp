#include "headers/mainwindow.h"
#include "ui_mainwindow.h"

#include "headers/section.h"

#include <QButtonGroup>
#include <QRadioButton>


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
    auto basic_sliders{create_section("Basic settings", {"Brightness", "Contrast", "Shadows", "Highlights", "Whites", "Blacks"})};
    auto advanced_sliders{create_section("Advanced settings", {"Structure", "Sharpen", "Vignette", "Blur", "Fade"})};
    auto color_sliders{create_section("Color settings", {"Saturation", "Luminance", "Temperature"})};
    // TODO: Add color selection
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
		if (0 == img.mType) {
			cv::cvtColor(img.mImg, tmp, cv::COLOR_BGR2GRAY);
			m_lb_image->setPixmap(QPixmap::fromImage(QImage(tmp.data, tmp.cols, tmp.rows, int(tmp.step), QImage::Format_Indexed8)));
		} else if (1 == img.mType) {
			cv::cvtColor(img.mImg, tmp, cv::COLOR_BGR2RGB);
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
		if (0 == img.mType)
			cv::cvtColor(tmp, tmp, cv::COLOR_BGR2GRAY);
		else if (1 == img.mType)
			cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGB);

		cv::imwrite(fileName, tmp);
	} catch (...) {
		QMessageBox::warning(this, "Warning", "Cannot save file" + QString::fromStdString(fileName));
	}
}

void MainWindow::on_action_Open_triggered()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Open file");
	try {
		cv::Mat tmp = cv::imread(fileName.toStdString());
		img = {tmp, fileName.toStdString()};
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
	ui->hlSide->addWidget(section);

    auto *vbox{new QVBoxLayout()};
    std::vector<QSlider*> sliders;

	for(auto &&e : contents)
	{
        vbox->addWidget(new QLabel(e, section));
        sliders.push_back(new QSlider(Qt::Horizontal, section));
        vbox->addWidget(sliders.back());
	}

    section->setContentLayout(*vbox);
    return sliders;
}

std::pair<std::vector<QSlider*>, QButtonGroup*> MainWindow::create_section(QString name, const std::vector<QString> &contents, int buttons)
{
    Section *section{new Section(name, 300, this)};
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

    section->setContentLayout(*vbox);
    return {sliders, toggle_group};
}

// TODO: Check if there is anything to save
void MainWindow::on_action_Save_triggered()
{
	if (m_has_image) {
		save_image(img.mFileName);
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
		cv::Mat tmp;
		cv::resize(img.mImg, tmp, cv::Size(), 1.1, 1.1);
		img.mImg = tmp;
		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded");
	}
}

// TODO: Fix zooming, implement dragging while zoomed [@milanilic332]
void MainWindow::on_action_ZoomOut_triggered()
{
	if (m_has_image) {
		cv::Mat tmp;
		cv::resize(img.mImg, tmp, cv::Size(), 0.9, 0.9);
		img.mImg = tmp;
		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded");
	}
}

void MainWindow::on_action_Mirror_triggered()
{
	if (m_has_image) {
		// TODO: push back to image ops
		cv::flip(img.mImg, img.mImg, 1);
		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded");
	}
}

void MainWindow::on_action_Rotate_left_triggered()
{
    if (m_has_image) {
		// TODO: push back to image ops
		cv::rotate(img.mImg, img.mImg, cv::ROTATE_90_COUNTERCLOCKWISE);
		show_image();
    } else {
        QMessageBox::warning(this, "Warning", "Image not loaded");
    }
}

void MainWindow::on_action_Rotate_right_triggered()
{
    if (m_has_image) {
		// TODO: push back to image ops
		cv::rotate(img.mImg, img.mImg, cv::ROTATE_90_CLOCKWISE);
		show_image();
    } else {
        QMessageBox::warning(this, "Warning", "Image not loaded");
    }
}


void MainWindow::on_btRGB_triggered()
{
	if (m_has_image) {
		img.mType = 1;
		show_image();
	} else {
		QMessageBox::warning(this, "Warning", "Image not loaded");
	}
}

void MainWindow::on_btGray_triggered()
{
	if (m_has_image) {
		img.mType = 0;
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
			cv::resize(img.mImg, img.mImg, cv::Size(fields[0]->text().toInt(), fields[1]->text().toInt()));
			show_image();
		}

	} else {
		QMessageBox::warning(this, "Warning", "Cannot crop image");
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
