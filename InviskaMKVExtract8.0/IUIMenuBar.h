#ifndef IUIMenuBar_h
#define IUIMenuBar_h

#include "IComUIMenuBarBase.h"


class IUIMenuBar : public IComUIMenuBarBase
{
    Q_OBJECT

private:
    // File Menu
    QAction*            m_pqactOpenFiles;
    QAction*            m_pqactOpenDir;

public:
    IUIMenuBar(IUIMainWindow* pmwMainWindow);

private:
    // Creates
    void CreateActions();

    // Creates menus and adds actions
    void InitialiseMenuBar();

public:
    // Returns credits for display in credits dialog
    virtual bool CreditsAvailable() {return true;}
    QString GetCreditsCodeContrib();
    QString GetCreditsDependency();
};

#endif // IUIMenuBar_h
