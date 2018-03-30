#pragma once
#include <QMainWindow>

class Mainwindow : public QMainWindow
{
	Q_OBJECT

public:
	Mainwindow()
		: edit(new QDirectoryEdit(this))
	{
		this->setCentralWidget(edit);
		edit->setLabelText("Directory:");
		edit->setDirectoryPath("C:/dev");
		edit->setPreviousDirectories(QStringList() << "C:/dev/bluetooth");
		connect(edit, &QDirectoryEdit::directoryChanged, [=](const QString& dir)
		{
			this->setWindowTitle(dir);
		});
	}

private:

	QDirectoryEdit * edit;
};

