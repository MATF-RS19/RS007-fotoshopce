#include "headers/mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Setting Widget params
    vlTools->setSpacing(5);
    vlTools->setAlignment(Qt::AlignTop);
    lbImage->setMinimumSize(600, 600);
    lbImage->setMaximumSize(600, 600);
    // Future use for cropping
    // lbImage->setMouseTracking(true);
    lbImage->setStyleSheet("QLabel { background-color : #CCCCCC; }");
    lbImage->setAlignment(Qt::AlignCenter);
    connect(btRGB, SIGNAL (clicked()), this, SLOT (on_btRGB_triggered()));
    connect(btGray, SIGNAL (clicked()), this, SLOT (on_btGray_triggered()));

    // Heirarchy
    hlMain->addWidget(lbImage);
    hlMain->addLayout(vlTools);
    vlTools->addLayout(hlImageType);
        hlImageType->addWidget(btRGB);
        hlImageType->addWidget(btGray);
    vlTools->addWidget(cbMainSettings);

    // Adding to centralWidget
    ui->centralWidget->setLayout(hlMain);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showImage()
{
    if (hasImage) {
        cv::Mat tmp;

        // Resizing (TODO: maybe completely new image)
        cv::resize(img.mImg, tmp, cv::Size(img.mWidth, img.mHeight));

        // Zoom (In conflict with resizing)
        // cv::resize(img.mImg, tmp, cv::Size(), img.mCurrentZoom, img.mCurrentZoom);

        // Mirror
        if (img.mMirrored)
            cv::flip(tmp, tmp, 1);

        // Type
        if (img.mType == 0) {
            cv::cvtColor(tmp, tmp, cv::COLOR_BGR2GRAY);
            lbImage->setPixmap(QPixmap::fromImage(QImage(tmp.data, tmp.cols, tmp.rows, int(tmp.step), QImage::Format_Indexed8)));
        }
        else if (img.mType == 1) {
            cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGB);
            lbImage->setPixmap(QPixmap::fromImage(QImage(tmp.data, tmp.cols, tmp.rows, int(tmp.step), QImage::Format_RGB888)));
        }
    }
    else {
        lbImage->clear();
    }
}

void MainWindow::saveImage(const std::string& fileName)
{
    try {
        cv::Mat tmp;

        // Zoom
        cv::resize(img.mImg, tmp, cv::Size(), img.mCurrentZoom, img.mCurrentZoom);

        // Mirror
        if (img.mMirrored)
            cv::flip(tmp, tmp, 1);

        // Type
        if (img.mType == 0)
            cv::cvtColor(tmp, tmp, cv::COLOR_BGR2GRAY);
        else if (img.mType == 1)
            cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGB);

        cv::imwrite(fileName, tmp);
    }
    catch (...) {
        QMessageBox::warning(this, "Warning", "Cannot save file" + QString::fromStdString(fileName));
    }
}

void MainWindow::on_action_Open_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open file");
    try {
       cv::Mat tmp = cv::imread(fileName.toStdString());
       img = {tmp, fileName.toStdString()};
       hasImage = true;
       showImage();
       setWindowTitle(fileName);
    }
    catch (...) {
        QMessageBox::warning(this, "Warning", "Cannot open file" + fileName);
    }
}

// TODO: Check if there is anything to save
void MainWindow::on_action_Save_triggered()
{
    if (hasImage) {
        saveImage(img.mFileName);
    }
    else {
        QMessageBox::warning(this, "Warning", "Image not loaded");
    }
}

void MainWindow::on_action_Save_as_triggered()
{
    if (hasImage) {
        QString fileName = QFileDialog::getSaveFileName(this, "Save as...");
        saveImage(fileName.toStdString());
    }
    else {
        QMessageBox::warning(this, "Warning", "Image not loaded");
    }
}

void MainWindow::on_action_Zoom_triggered()
{
//    if (hasImage) {
//        if (img.mCurrentZoom < 2.0)
//            img.mCurrentZoom += 0.1;
//        showImage();
//    }
//    else {
//        QMessageBox::warning(this, "Warning", "Image not loaded");
//    }
}

void MainWindow::on_action_Zoom_out_triggered()
{
//    if (hasImage) {
//        if (img.mCurrentZoom > 0.2)
//            img.mCurrentZoom -= 0.1;
//        showImage();
//    }
//    else {
//        QMessageBox::warning(this, "Warning", "Image not loaded");
//    }
}

void MainWindow::on_action_Mirror_triggered()
{
    if (hasImage) {
        img.mMirrored = !img.mMirrored;
        showImage();
    }
    else {
        QMessageBox::warning(this, "Warning", "Image not loaded");
    }
}


void MainWindow::on_btRGB_triggered()
{
    if (hasImage) {
        img.mType = 1;
        showImage();
    }
    else {
        QMessageBox::warning(this, "Warning", "Image not loaded");
    }
}

void MainWindow::on_btGray_triggered()
{
    if (hasImage) {
        img.mType = 0;
        showImage();
    }
    else {
        QMessageBox::warning(this, "Warning", "Image not loaded");
    }
}

void MainWindow::on_action_Delete_triggered()
{
    if (hasImage) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Warning", "Are you sure you want to delete current image?",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            img = {};
            hasImage = false;
            showImage();
        }
    }
    else {
        QMessageBox::warning(this, "Warning", "Nothing to delete");
    }
}

void MainWindow::on_action_Resize_triggered()
{
    if (hasImage) {
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
            img.mWidth = fields[0]->text().trimmed().toInt();
            img.mHeight = fields[1]->text().trimmed().toInt();
            showImage();
        }
    }
    else {
        QMessageBox::warning(this, "Warning", "Cannot crop image");
    }
}

void MainWindow::on_action_Exit_triggered()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Warning", "Are you sure you want to exit?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QApplication::quit();
    }
}
