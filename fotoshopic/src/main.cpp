#include "headers/mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QFile File(":/icons/QTDark.stylesheet");
	File.open(QFile::ReadOnly);
	QString StyleSheet = QLatin1String(File.readAll());

	qApp->setStyleSheet(StyleSheet);
	MainWindow w;
	w.show();

	return a.exec();
}
