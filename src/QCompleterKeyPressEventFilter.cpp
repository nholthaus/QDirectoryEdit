#include "QCompleterKeyPressEventFilter.h"

// --------------------------------------------------------------------------------
// 	FUNCTION: QCompleterKeyPressEventFilter (public )
// --------------------------------------------------------------------------------
QCompleterKeyPressEventFilter::QCompleterKeyPressEventFilter(QObject *c_watch, Qt::Key key, QObject *parent /*= 0*/)
{
    m_watch = c_watch;
    m_key = key;
}
// --------------------------------------------------------------------------------
// 	FUNCTION: eventFilter (protected )
// --------------------------------------------------------------------------------
bool QCompleterKeyPressEventFilter::eventFilter(QObject *p_obj, QEvent *p_event)
{
    if (p_event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(p_event);
        if (keyEvent->key() == m_key)
        {
            handleEvent(p_obj);
            return true;
        }
    }

    return false;
}
// --------------------------------------------------------------------------------
// 	FUNCTION: handleEvent (protected )
// --------------------------------------------------------------------------------
void QCompleterKeyPressEventFilter::handleEvent(QObject* p_obj)
{
    QAbstractItemView* comp = static_cast<QAbstractItemView*>(p_obj);

    if (!comp->currentIndex().isValid())
    {
        // if nothing is selected, push down then enter
        QKeyEvent key(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier);
        QApplication::sendEvent(p_obj, &key);

        key = QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
        QApplication::sendEvent(p_obj, &key);
    }
    else
    {
        // if something is selected, just push enter
        QKeyEvent key(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
        QApplication::sendEvent(p_obj, &key);
    }
}
