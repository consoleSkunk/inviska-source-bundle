#ifndef IComDlgHelpAbout_h
#define IComDlgHelpAbout_h

#include <QDialog>
class QDialogButtonBox;
class IUIMainWindow;


class IComDlgHelpAbout : public QDialog
{
    Q_OBJECT

private:
    // Main window
    IUIMainWindow*                  m_pmwMainWindow;

    // About Qt button - mostly so the user can check what version of Qt is being used
    QPushButton*                    m_pqpbAboutQt;

    // OK button
    QDialogButtonBox*               m_pqdbbOK;

public:
    IComDlgHelpAbout(IUIMainWindow* pmwMainWindow);

private slots:
    // Displays About Qt messagebox
    void ShowAboutQt();
};

#endif // IComDlgHelpAbout_h
