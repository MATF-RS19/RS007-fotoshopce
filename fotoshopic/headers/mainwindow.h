#pragma once

#include <vector>

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
#include "utils.h"

template <typename T>
using qstring_map = std::unordered_map<QString, T>;

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
	Q_OBJECT

	// Public member functions
	public:
		explicit MainWindow(QWidget *parent = nullptr);
		~MainWindow();
	// Private slots
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
		void on_action_Rotate_left_triggered();
		void on_action_Rotate_right_triggered();
        void on_action_Undo_triggered();
        void on_action_Redo_triggered();
	// Private member functions
    private:
		void show_image() const;
		void save_image(const std::string& fileName);
//      void push_operation(fs::ops::AbstractOperation *op);
//		void slider_operation(qstring_map<QSlider*> &sliders, const QString &key, fs::ops::basic_edits edit, int value = 50);
        void pop_operation();
		qstring_map<QSlider*> create_section(QString name, const std::vector<QString> &contents);
		std::pair<qstring_map<QSlider*>, QButtonGroup*> create_section(QString name, const std::vector<QString> &contents, int buttons);
		void delete_after_redo();
	// Private variables
	private:
		// Define the image
		QLabel* m_lb_image;
		Ui::MainWindow *ui;
		Image img;
//		std::vector<std::unique_ptr<fs::ops::AbstractOperation>> m_fwd_ops, m_bwd_ops;
		bool m_has_image;
		qstring_map<std::pair<int, int>> m_adjustment_map;
		std::vector<Section*> m_sections;
		std::vector<Image> image_list;
		unsigned long index;
};

