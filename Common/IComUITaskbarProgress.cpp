#include <QTimer>
#include "IComQLineEdit.h"


IComQLineEdit::IComQLineEdit(QWidget* pqwParent, bool bSelectAllOnActivate) : QLineEdit(pqwParent)
{
    m_bSelectAllOnActivate = bSelectAllOnActivate;
}


void IComQLineEdit::focusInEvent(QFocusEvent* pqfeFocusEvent)
{
    if (m_bSelectAllOnActivate)
        QTimer::singleShot(0, this, SLOT(selectAll()));

    emit GotFocus();

    QLineEdit::focusInEvent(pqfeFocusEvent);
}
