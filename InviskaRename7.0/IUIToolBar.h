#ifndef IUIToolBar_h
#define IUIToolBar_h

#include <QCoreApplication>
class IUIMainWindow;
class QToolBar;
class IComQLineEdit;


class IUIToolBar
{
    Q_DECLARE_TR_FUNCTIONS(IUIToolBar)

private:
    // Pointer to toolbar
    QToolBar*                       m_pqtpNavigationToolBar;

    // Address bar for displaying and entering directory path
    IComQLineEdit*                  m_pqleAddressBox;

public:
    IUIToolBar(IUIMainWindow* pmwMainWindow);

    // Gives the address bar focus
    void FocusAddressBar();

    // Selects all text in the address bar
    void AddressBarSelectAll();

    // Accessor fuctions
    QString GetAdddressBarText();
    void SetAddressBarText(const QString & krstrPath);
};

#endif // IUIToolBar_h
