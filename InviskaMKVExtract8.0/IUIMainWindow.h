#ifndef IUIMainWindow_h
#define IUIMainWindow_h

#define DFLT_WIN_WIDTH      520
#define DFLT_WIN_HEIGHT     630

#include "IComUIMainWinBase.h"
class IUIExtract;


class IUIMainWindow : public IComUIMainWinBase
{
    Q_OBJECT

private:
    // Static pointer to self for easy access to main window from anywhere in the program
    static IUIMainWindow*       m_spmwMainWindow;

    // Main UI for adding files and extracting tracks
    IUIExtract*                 m_puiextExtractUI;

public:
    IUIMainWindow(QSettings & rqsetSettings, IComSysSingleInstance & rsnglSingleInstance);
    ~IUIMainWindow();

public:
    // Called once the MKVTooNix version has been determined, this initialises processing of command line parameters
    void ProcessCommandLineParameters();

    // Accessor functions
    IUIExtract* & GetExtractUI()                    {return m_puiextExtractUI;}

    // Static accessor to main window for easy access from anywhere in the program
    static IUIMainWindow* GetMainWindow()           {return IUIMainWindow::m_spmwMainWindow;}
};

#endif // IUIMainWindow_h
