#ifndef IComQLineEdit_h
#define IComQLineEdit_h

#include <QLineEdit>

// QLineEdit that selects all text when activated


class IComQLineEdit : public QLineEdit
{
    Q_OBJECT

private:
    // Enables/disables feature where all text in the line edit is selected when the line edit receives focus
    bool        m_bSelectAllOnActivate;

public:
    IComQLineEdit(QWidget* pqwParent = nullptr, bool bSelectAllOnActivate = false);

protected:
    // Overrides function so it can emit the GotFocus() and apply the SelectAllOnActivate feature
    void focusInEvent(QFocusEvent* pqfeFocusEvent);

public:
    // Used to enable/disable the select all on activate feature
    void SetSelectAllOnActivate(const bool kbEnabled)   {m_bSelectAllOnActivate = kbEnabled;}

signals:
    // Signal that is emited when the line edit receives focus
    void GotFocus();
};

#endif // IComQLineEdit_h
