#ifndef IUIMainWindow_h
#define IUIMainWindow_h

#define DFLT_WIN_WIDTH      1000
#define DFLT_WIN_HEIGHT     720

#include "IComUIMainWinBase.h"
class QVBoxLayout;
class IUIFileList;
class IUIRenameSettings;
class IUIToolBar;
class IUISideBar;


class IUIMainWindow : public IComUIMainWinBase
{
    Q_OBJECT

private:
    // Static pointer to self for easy access to main window from anywhere in the program
    static IUIMainWindow*       m_spmwMainWindow;

    // File list which displays the files' current and preivew name and handles file renaming
    IUIFileList*                m_puifmFileList;

    // Bottom rename settings widget
    IUIRenameSettings*          m_puirsRenameSettingsUI;

    // Navigation toolbar for navigating between directories
    IUIToolBar*                 m_puitbToolBar;

    // Side bar with arrows for re-ordering file list
    IUISideBar*                 m_puisbSideBar;

public:
    IUIMainWindow(QSettings & rqsetSettings, IComSysSingleInstance & rsnglSingleInstance);
    ~IUIMainWindow();

private slots:
    // Called by a zero ms timer, this gives the address bar focus once everything else is created
    void FocusAddressBar();

public:
    // Enables or disables side bar
    void SetSideBarEnabled(const bool kbEnabled);
    bool SideBarEnabled();

    // Accessor functions
    IUIFileList* &          GetFileListUI()         {return m_puifmFileList;}
    IUIRenameSettings* &    GetRenameSettingsUI()   {return m_puirsRenameSettingsUI;}
    IUIToolBar* &           GetToolBar()            {return m_puitbToolBar;}

    // Static accessor to main window for easy access from anywhere in the program
    static IUIMainWindow*   GetMainWindow()         {return IUIMainWindow::m_spmwMainWindow;}
};

#endif // IUIMainWindow_h
