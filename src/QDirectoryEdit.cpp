//------------------------------
//	INCLUDES
//------------------------------

#include "QDirectoryEdit.h"

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QFileSystemModel>
#include <QCompleter>
#include <QCompleterKeyPressEventFilter.h>
#include <QTimer>
#include <QDir>

//--------------------------------------------------------------------------------------------------
//	QDirectoryEditPrivate
//--------------------------------------------------------------------------------------------------

class QDirectoryEditPrivate
{
public:

	QDirectoryEditPrivate() 
		: layout(new QHBoxLayout)
		, label(new QLabel)
		, browseButton(new QPushButton)
		, fileSystemModel(new QFileSystemModel)
		, edit(new QLineEdit)
		, completionTimer(new QTimer)
	{
		layout->addWidget(label);
		layout->addWidget(edit);
		layout->addWidget(browseButton);

		edit->setPlaceholderText("Directory Path...");
		browseButton->setText("Browse...");

		label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
		browseButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

		// File system text completer
		QCompleter* fileSystemCompleter = new QCompleter(fileSystemModel);
		fileSystemCompleter->setCaseSensitivity(Qt::CaseInsensitive);
		fileSystemCompleter->setCompletionMode(QCompleter::PopupCompletion);
		fileSystemCompleter->popup()->installEventFilter(new QCompleterKeyPressEventFilter(fileSystemCompleter, Qt::Key_Tab));

		fileSystemModel->setRootPath("");
		fileSystemModel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);

		// path line edit
		edit->setCompleter(fileSystemCompleter);

		completionTimer->setSingleShot(true);
	}

	~QDirectoryEditPrivate()
	{
		delete fileSystemModel;

		// the layout/widgets will be deleted by the QDirectoryEdit which
		// parents them
	}

public:

	QHBoxLayout*		layout;
	QLabel*				label;
	QPushButton*		browseButton;
	QFileSystemModel*	fileSystemModel;
	QLineEdit*			edit;
	QTimer*				completionTimer;

#ifdef WIN32
	QChar slash = QChar('\\');
#else
	QChar slash = QChar('/');
#endif
};

//--------------------------------------------------------------------------------------------------
//	QDirectoryEdit (public ) []
//--------------------------------------------------------------------------------------------------
QDirectoryEdit::QDirectoryEdit(QWidget* parent /*= nullptr*/)
	: QWidget(parent)
	, d_ptr(new QDirectoryEditPrivate)
{
	Q_D(QDirectoryEdit);

	this->setLayout(d->layout);

	auto complete = [=]()
	{
		d->edit->completer()->setCompletionPrefix(d->edit->text());
		d->edit->completer()->complete();
		d->completionTimer->start(10);
	};

	auto appendSlashes = [=]()
	{
		QModelIndex fileSystemIndex = d->fileSystemModel->index(d->edit->text());
		QModelIndex index = fileSystemIndex;

		if (d->fileSystemModel->isDir(fileSystemIndex) && fileSystemIndex.isValid())
			if (!d->edit->text().endsWith(d->slash, Qt::CaseInsensitive))
				d->edit->setText(d->edit->text().append(d->slash));

		complete();
	};

	connect(d->edit->completer(), QOverload<const QModelIndex&>::of(&QCompleter::activated), [=](QModelIndex index)
	{
		QTimer* timer = new QTimer(this);
		timer->setSingleShot(true);
		timer->setInterval(10);
		connect(timer, &QTimer::timeout, appendSlashes);
		timer->start();
	});

	connect(d->edit, &QLineEdit::editingFinished, [=]()
	{
		if (this->isValid())
		{
			emit this->directoryChanged(d->edit->text());
			QPalette palette;
			palette.setColor(QPalette::Base, Qt::white);
			d->edit->setPalette(palette);
		}
		else
		{
			QPalette palette;
			palette.setColor(QPalette::Base, QColor("#ffcccc"));
			d->edit->setPalette(palette);
		}
	});

	connect(d->edit, &QLineEdit::textChanged, [=](QString text)
	{
		// change to platform-specific slashes
		if (d->slash == QChar('/'))
			if (d->edit->text().contains('\\', Qt::CaseInsensitive))
				d->edit->setText(d->edit->text().replace('\\', d->slash));
		if(d->slash == QChar('\\'))
			if (d->edit->text().contains('/', Qt::CaseInsensitive))
				d->edit->setText(d->edit->text().replace('/', d->slash));
	});

	connect(d->edit->completer(), QOverload<const QString&>::of(&QCompleter::activated), [=](QString text)
	{
		// append slashes to dir
		appendSlashes();
	});

	connect(d->browseButton, &QPushButton::clicked, this, [=]()
	{
		QString dir = QFileDialog::getExistingDirectory(this, "Choose Directory", d->edit->text());
		this->setDirectoryPath(dir);
	});

	connect(d->completionTimer, &QTimer::timeout, d->edit->completer(), [=]()
	{
		d->edit->completer()->complete();
	}, Qt::QueuedConnection);

	// set the root dir to the first available drive (usually c:\)
	if (!QDir::drives().isEmpty())
		d->edit->setText(QDir::drives().first().absolutePath());
}

//--------------------------------------------------------------------------------------------------
//	~QDirectoryEdit () []
//--------------------------------------------------------------------------------------------------
QDirectoryEdit::~QDirectoryEdit()
{

}

//--------------------------------------------------------------------------------------------------
//	directoryPath (public ) []
//--------------------------------------------------------------------------------------------------
QString QDirectoryEdit::directoryPath() const noexcept
{
	Q_D(const QDirectoryEdit);
	return d->edit->text().replace('\\','/');
}

//--------------------------------------------------------------------------------------------------
//	isEmpty () []
//--------------------------------------------------------------------------------------------------
bool QDirectoryEdit::isEmpty() const noexcept
{
	Q_D(const QDirectoryEdit);
	return d->edit->text().isEmpty();
}

//--------------------------------------------------------------------------------------------------
//	isValid () []
//--------------------------------------------------------------------------------------------------
bool QDirectoryEdit::isValid() const noexcept
{
	Q_D(const QDirectoryEdit);

	QModelIndex fileSystemIndex = d->fileSystemModel->index(d->edit->text());
	QModelIndex index = fileSystemIndex;

	if (d->fileSystemModel->isDir(fileSystemIndex) && fileSystemIndex.isValid())
		return true;
	else
		return false;
}

//--------------------------------------------------------------------------------------------------
//	labelText () []
//--------------------------------------------------------------------------------------------------
QString QDirectoryEdit::labelText() const noexcept
{
	Q_D(const QDirectoryEdit);
	return d->label->text();
}

//--------------------------------------------------------------------------------------------------
//	setDirectoryPath () []
//--------------------------------------------------------------------------------------------------
bool QDirectoryEdit::setDirectoryPath(const QString& path) noexcept
{
	Q_D(QDirectoryEdit);

	QModelIndex fileSystemIndex = d->fileSystemModel->index(path);
	QModelIndex index = fileSystemIndex;

	if (d->fileSystemModel->isDir(fileSystemIndex) && fileSystemIndex.isValid())
	{
		d->edit->setText(path);
		if (!d->edit->text().endsWith(d->slash, Qt::CaseInsensitive))
			d->edit->setText(d->edit->text().append(d->slash));
		this->update();
	}
	else
		return false;

	return true;
}

//--------------------------------------------------------------------------------------------------
//	setLabelText () []
//--------------------------------------------------------------------------------------------------
void QDirectoryEdit::setLabelText(const QString& text) noexcept
{
	Q_D(QDirectoryEdit);
	d->label->setText(text);
}

