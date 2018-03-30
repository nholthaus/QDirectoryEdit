//--------------------------------------------------------------------------------------------------
// 
//	@project	QDirectoryEdit
//
//--------------------------------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), to deal in the Software without 
// restriction, including without limitation the rights to use, copy, modify, merge, publish, 
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or 
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//--------------------------------------------------------------------------------------------------
//
// Copyright (c) 2018 Nic Holthaus
// 
//--------------------------------------------------------------------------------------------------
//
// ATTRIBUTION:
//
//
//--------------------------------------------------------------------------------------------------
//
/// @file	QDirectoryEdit.h
/// @brief	An edit widget to select a directory, with browse, tab complete, and validation
//
//--------------------------------------------------------------------------------------------------

#pragma once
#ifndef QDirectoryEdit_h__
#define QDirectoryEdit_h__

//-------------------------
//	INCLUDES
//-------------------------

#include <QWidget> 

//-------------------------
//	FORWARD DECLARATIONS
//-------------------------

class QDirectoryEditPrivate;

//--------------------------------------------------------------------------------------------------
//	QDirectoryEdit
//--------------------------------------------------------------------------------------------------

class QDirectoryEdit : public QWidget
{
	Q_OBJECT

public:

	QDirectoryEdit(QWidget* parent = nullptr);
	virtual ~QDirectoryEdit();
	
signals:

	void directoryChanged(const QString& dir);

public:

	QString directoryPath() const noexcept;
	bool isEmpty() const noexcept;
	bool isValid() const noexcept;
	QString labelText() const noexcept;
	bool setDirectoryPath(const QString& path) noexcept;
	void setLabelText(const QString& text) noexcept;
	
private:

	Q_DECLARE_PRIVATE(QDirectoryEdit);
	Q_DISABLE_COPY(QDirectoryEdit);
	QScopedPointer<QDirectoryEditPrivate> d_ptr;

};

#endif // QDirectoryEdit_h__
