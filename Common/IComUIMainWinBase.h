#ifndef IComUIMainWinBase_h
#define IComUIMainWinBase_h

#include <QMainWindow>
#include <QTimer>
#include "IComSysLatestVersion.h"
class QSettings;
class IComSysSingleInstance;
class IUIMenuBar;


class IComUIMainWinBase : public QMainWindow
{
    Q_OBJECT

protected:
    // Application settings
    QSettings &                 m_rqsetSettings;

    // Reference to single instance object and timer used to periodically check if another instance has been started
    IComSysSingleInstance &     m_rsnglSingleInstance;
    QTimer                      m_qtimOtherInstanceCheck;

    // Stores inforamtion about the latest version of the software
    IComSysLatestVersion        m_ilvLatestVersion;

    // Menu bar
    IUIMenuBar*                 m_puimbMenuBar;

public:
    IComUIMainWinBase(QSettings & rqsetSettings, IComSysSingleInstance & rsnglSingleInstance);
    virtual ~IComUIMainWinBase();

protected:
    // Actions to be performed before application closes
    virtual void closeEvent(QCloseEvent* qceEvent);

private:
    // Load and save window position settings on startup/shutdown
    virtual void ReadSettings();
    virtual void SaveSettings();

public:
    // Returns true if the window is within the bounds of the desktop, accounting for multi-monitor setups
    bool IsWindowOnScreen(const QRect & krqrctWinRect);

private slots:
    // Checks shared memory for other instances of application and
    void CheckForOtherInstances();

public:
    // Functions to return the application name, version number and website
    static QString GetVersion()             {return APP_VERSION;}
    static QString GetAppName()             {return APP_NAME;}
    static QString GetAppNameNoSpaces()     {return APP_NAME_NO_SPACES;}
    QString GetWebsite();
    QString GetForumAddress();
    QString GetChangelogAddress();

    // Accessor functions
    IUIMenuBar* &         GetMenuBar()      {return m_puimbMenuBar;}
    QSettings &           GetSettings()     {return m_rqsetSettings;}
    IComSysLatestVersion* GetVersionInfo()  {return &m_ilvLatestVersion;}
};

#endif // IComUIMainWinBase_h
