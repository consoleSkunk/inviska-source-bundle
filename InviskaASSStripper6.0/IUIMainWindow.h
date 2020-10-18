#ifndef IUIMainWindow_h
#define IUIMainWindow_h

#define DFLT_WIN_WIDTH      500
#define DFLT_WIN_HEIGHT     630

#include "IComUIMainWinBase.h"
class IUIStripper;


class IUIMainWindow : public IComUIMainWinBase
{
    Q_OBJECT

private:
    // Static pointer to self for easy access to main window from anywhere in the program
    static IUIMainWindow*       m_spmwMainWindow;

    // Widgets for configuring strip settings
    IUIStripper*                m_puisStripControls;

public:
    IUIMainWindow(QSettings & rqsetSettings, IComSysSingleInstance & rsnglSingleInstance);
    ~IUIMainWindow();

public:
    // Accessor functions
    IUIStripper* & GetStripperUI()                  {return m_puisStripControls;}

    // Static accessor to main window for easy access from anywhere in the program
    static IUIMainWindow* GetMainWindow()           {return IUIMainWindow::m_spmwMainWindow;}
};

#endif // IUIMainWindow_h
