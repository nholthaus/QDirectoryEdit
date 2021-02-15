//------------------------------
//	INCLUDES
//------------------------------

#include "QDirectoryEdit.h"

#include <QComboBox>
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
		, edit(new QComboBox)
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
		edit->setEditable(true);
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
	QComboBox*			edit;
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
		d->edit->completer()->setCompletionPrefix(d->edit->currentText());
		d->edit->completer()->complete();
		d->completionTimer->start(10);
	};

	auto appendSlashes = [=]()
	{
		QModelIndex fileSystemIndex = d->fileSystemModel->index(d->edit->currentText());
		
		if (d->fileSystemModel->isDir(fileSystemIndex) && fileSystemIndex.isValid())
			if (!d->edit->currentText().endsWith(d->slash, Qt::CaseInsensitive))
				d->edit->setCurrentText(d->edit->currentText().append(d->slash));

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

	connect(d->edit, &QComboBox::currentTextChanged, [=]()
	{
		if (this->isValid())
		{
			emit this->directoryChanged(directoryPath());
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

	connect(d->edit, &QComboBox::currentTextChanged, [=](QString text)
	{
		// change to platform-specific slashes
		if (d->slash == QChar('/'))
			if (d->edit->currentText().contains('\\', Qt::CaseInsensitive))
				d->edit->setCurrentText(d->edit->currentText().replace('\\', d->slash));
		if(d->slash == QChar('\\'))
			if (d->edit->currentText().contains('/', Qt::CaseInsensitive))
				d->edit->setCurrentText(d->edit->currentText().replace('/', d->slash));
	});

	connect(d->edit->completer(), QOverload<const QString&>::of(&QCompleter::activated), [=](QString text)
	{
		// append slashes to dir
		appendSlashes();
	});

	connect(d->browseButton, &QPushButton::clicked, this, [=]()
	{
		QString dir = QFileDialog::getExistingDirectory(this, "Choose Directory", d->edit->currentText());
		this->setDirectoryPath(dir);
		d->edit->addItem(dir);
	});

	connect(d->completionTimer, &QTimer::timeout, [=]()
	{
		d->edit->completer()->complete();
	});

	// set the root dir to the first available drive (usually c:\)
	if (!QDir::drives().isEmpty())
		d->edit->setCurrentText(QDir::drives().first().absolutePath());
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
	auto path = d->edit->currentText();
	if (!path.endsWith(d->slash))
		path.append(d->slash);
	return path.replace('\\','/');
}

//--------------------------------------------------------------------------------------------------
//	isEmpty () []
//--------------------------------------------------------------------------------------------------
bool QDirectoryEdit::isEmpty() const noexcept
{
	Q_D(const QDirectoryEdit);
	return d->edit->currentText().isEmpty();
}

//--------------------------------------------------------------------------------------------------
//	isValid () []
//--------------------------------------------------------------------------------------------------
bool QDirectoryEdit::isValid() const noexcept
{
	Q_D(const QDirectoryEdit);

	QModelIndex fileSystemIndex = d->fileSystemModel->index(d->edit->currentText());

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
//	previousDirectories (public ) []
//--------------------------------------------------------------------------------------------------
QStringList QDirectoryEdit::previousDirectories() const noexcept
{
	Q_D(const QDirectoryEdit);

	QStringList sl;
	for (int i = 0; i < d->edit->count(); ++i)
		sl << d->edit->itemText(i);

	return sl;
}

//--------------------------------------------------------------------------------------------------
//	setDirectoryPath () []
//--------------------------------------------------------------------------------------------------
bool QDirectoryEdit::setDirectoryPath(const QString& path) noexcept
{
	Q_D(QDirectoryEdit);

	QModelIndex fileSystemIndex = d->fileSystemModel->index(path);

	if (d->fileSystemModel->isDir(fileSystemIndex) && fileSystemIndex.isValid())
	{
		d->edit->addItem(path);
		d->edit->setCurrentText(path);
		if (!d->edit->currentText().endsWith(d->slash, Qt::CaseInsensitive))
			d->edit->setCurrentText(d->edit->currentText().append(d->slash));
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

//--------------------------------------------------------------------------------------------------
//	setPreviousDirectories (public ) []
//--------------------------------------------------------------------------------------------------
void QDirectoryEdit::setPreviousDirectories(const QStringList& directories) noexcept
{
	Q_D(QDirectoryEdit);

	for (auto dir : directories)
	{
		QModelIndex fileSystemIndex = d->fileSystemModel->index(dir);

		if (d->fileSystemModel->isDir(fileSystemIndex) && fileSystemIndex.isValid())
			d->edit->addItem(dir);
	}
}

