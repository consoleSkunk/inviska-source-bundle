#ifndef IUIMenuBar_h
#define IUIMenuBar_h

#include "IComUIMenuBarBase.h"


class IUIMenuBar : public IComUIMenuBarBase
{
    Q_OBJECT

private:
    // File Menu
    QAction*            m_pqactOpenFiles;

public:
    IUIMenuBar(IUIMainWindow* pmwMainWindow);

private:
    // Creates
    void CreateActions();

    // Creates menus and adds actions
    void InitialiseMenuBar();
};

#endif // IUIMenuBar_h
