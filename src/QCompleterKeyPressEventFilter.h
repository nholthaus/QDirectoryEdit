#ifndef QCompleterKeyPressEventFilter_h__
#define QCompleterKeyPressEventFilter_h__

//------------------------
//	INCLUDES
//------------------------
#include <QObject>
#include <QWidget>
#include <QEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QAbstractItemView>

//	--------------------------------------------------------------------------------
///	@class		QKeyPressEventFilter
///	@brief
///	@details
//  --------------------------------------------------------------------------------
class QCompleterKeyPressEventFilter : public QObject
{
    Q_OBJECT

public:

    QCompleterKeyPressEventFilter(QObject *c_watch, Qt::Key key, QObject *parent = 0);

protected:

    bool eventFilter(QObject *p_obj, QEvent *p_event);
    virtual void handleEvent(QObject* p_obj);

private:

    QObject* m_watch;
    Qt::Key  m_key;

};

#endif // QCompleterKeyPressEventFilter_h__
