#ifndef IComDlgCredits_h
#define IComDlgCredits_h

#include <QDialog>
class QDialogButtonBox;
class QVBoxLayout;
class IUIMainWindow;


class IComDlgCredits : public QDialog
{
    Q_OBJECT

private:
    // Main window
    IUIMainWindow*                  m_pmwMainWindow;

    // OK button
    QPushButton*                    m_pqpbOK;

public:
    IComDlgCredits(IUIMainWindow* pmwMainWindow);

    // Adds a label with the passed credits
    void AddLabel(QVBoxLayout* pqvblLayout, const QString & krqstrTitle, const QString & krqstrCredits);
};

#endif // IComDlgCredits_h
