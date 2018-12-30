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

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "image.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    friend Image;
private slots:
    void on_action_Zoom_triggered();
    void on_action_Zoom_out_triggered();
    void on_btRGB_triggered();
    void on_btGray_triggered();
    void on_action_Mirror_triggered();

    void on_action_Open_triggered();

    void on_action_Save_triggered();

    void on_action_Save_as_triggered();

private:
    // Defining Layouts
    QHBoxLayout* hlMain = new QHBoxLayout();
    QVBoxLayout* vlTools = new QVBoxLayout();
    QHBoxLayout* hlImageType = new QHBoxLayout();

    // Defining Widgets
    QPushButton* btRGB = new QPushButton("RGB");
    QPushButton* btGray = new QPushButton("Gray");
    QLabel* lbImage = new QLabel();
    QComboBox* cbMainSettings = new QComboBox();

    Ui::MainWindow *ui;

    // Image class
    Image img{};

    void showImage();
    void saveImage(const std::string& fileName);
};

#endif // MAINWINDOW_H
