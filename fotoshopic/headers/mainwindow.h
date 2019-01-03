#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QFormLayout>
#include <QDialog>
#include <QLineEdit>
#include <QList>
#include <QDialogButtonBox>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "image.h"
#include "section.h"


namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

	/* Public member functions */
	public:
		explicit MainWindow(QWidget *parent = nullptr);
		~MainWindow();
	/* Private slots */
	private slots:
		void on_action_ZoomIn_triggered();
		void on_action_ZoomOut_triggered();
		void on_btRGB_triggered();
		void on_btGray_triggered();
		void on_action_Mirror_triggered();
		void on_action_Open_triggered();
		void on_action_Save_triggered();
		void on_action_SaveAs_triggered();
		void on_action_Delete_triggered();
		void on_action_Exit_triggered();
		void on_action_Resize_triggered();
	/* Private member functions */
	private:
		void show_image() const;
		void save_image(const std::string& fileName);
		void create_section(QString name, const std::vector<QString> &contents);
	/* Private variables */
	private:
		// Define the image
		QLabel* m_lb_image;
		Ui::MainWindow *ui;
		Image img;
		bool m_has_image;
};

#endif // MAINWINDOW_H
