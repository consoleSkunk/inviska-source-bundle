#include <QtWidgets>
#include <QSettings>
#include "IComUIMainWinBase.h"
#include "IComSysLatestVersion.h"
#include "IComSysSingleInstance.h"
#include "IUIMainWindow.h"
#include "IUIMenuBar.h"


IComUIMainWinBase::IComUIMainWinBase(QSettings & rqsetSettings, IComSysSingleInstance & rsnglSingleInstance) : m_rqsetSettings(rqsetSettings),
                                                                                                               m_rsnglSingleInstance(rsnglSingleInstance),
                                                                                                               m_ilvLatestVersion(static_cast<IUIMainWindow*>(this))
{
    m_puimbMenuBar = nullptr;
    setWindowTitle(GetAppName());

    // On Windows and Mac the icon is set in the .pro file with RC_ICONS and ICON respectively, so ther's no need to set the window icon here.
    // Some Linux desktop environments set the window icon from the .desktop file, but others don't appear to, which is why we set it here on Linux.
    #if !defined(Q_OS_WIN) && !defined(Q_OS_MACOS)
    setWindowIcon(QIcon(":/Resources/Icon.png"));
    #endif

    ReadSettings();

    m_qtimOtherInstanceCheck.setParent(this);
    if (m_rqsetSettings.value("Application/SingleInstanceOnly", false).toBool())
    {
        connect(&m_qtimOtherInstanceCheck, SIGNAL(timeout()), this, SLOT(CheckForOtherInstances()));
        m_qtimOtherInstanceCheck.start(400);
    }
}


IComUIMainWinBase::~IComUIMainWinBase()
{
    m_qtimOtherInstanceCheck.stop();
}


void IComUIMainWinBase::closeEvent(QCloseEvent* /*qceEvent*/)
{
    SaveSettings();
}


void IComUIMainWinBase::ReadSettings()
{
    m_rqsetSettings.beginGroup("MainWindow");
    bool bUseSpecifiedSize = m_rqsetSettings.value("UseSpecifiedSizePos", false).toBool();
    resize(m_rqsetSettings.value(bUseSpecifiedSize ? "UserSpecifiedSize" : "LastSize", QSize(DFLT_WIN_WIDTH, DFLT_WIN_HEIGHT)).toSize());
    move(m_rqsetSettings.value(bUseSpecifiedSize ? "UserSpecifiedPosition" : "LastPosition", QGuiApplication::screens().first()->geometry().center() - rect().center()).toPoint());
    m_rqsetSettings.endGroup();

    if (IsWindowOnScreen(geometry()) == false)
        move(QGuiApplication::screens().first()->geometry().center() - rect().center());
}


bool IComUIMainWinBase::IsWindowOnScreen(const QRect & krqrctWinRect)
{
    /* Main Monitor:	 QRect(0,0 1920x1200)
     * Multi-Mon Res:	 QRect(-1920,0 3840x1200)
    qDebug() << "Main Monitor:\t" << QApplication::desktop()->screenGeometry(); //QApplication::desktop()->screenGeometry(QApplication::desktop()->primaryScreen())
    qDebug() << "Multi-Mon Res:\t" << QApplication::desktop()->geometry();*/

    const int kiMinPixOnScreen = 20;
    QRect qrctDesktop = QApplication::desktop()->geometry();

    // Confirm left side of window isn't off right side of desktop
    if (krqrctWinRect.x() > qrctDesktop.x() + qrctDesktop.width() - kiMinPixOnScreen)
        return false;

    // Confirm right side of window isn't off left side of desktop
    if (krqrctWinRect.x() + krqrctWinRect.width() < qrctDesktop.x() + kiMinPixOnScreen)
        return false;

    // Confirm top of window isn't off bottom of desktop
    if (krqrctWinRect.y() > qrctDesktop.y() + qrctDesktop.height() - kiMinPixOnScreen)
        return false;

    // Confirm top of window isn't off top of desktop
    if (krqrctWinRect.y() < qrctDesktop.y())
        return false;

    return true;
}


void IComUIMainWinBase::SaveSettings()
{
    m_rqsetSettings.beginGroup("MainWindow");
    m_rqsetSettings.setValue("LastSize", size());
    m_rqsetSettings.setValue("LastPosition", pos());
    m_rqsetSettings.endGroup();
}


void IComUIMainWinBase::CheckForOtherInstances()
{
    if (m_rsnglSingleInstance.CheckInstanceFlag())
        activateWindow();
}


QString IComUIMainWinBase::GetWebsite()
{
    QString qstrAppName = GetAppNameNoSpaces();
    qstrAppName.remove("Inviska");
    return "https://www.inviska.com/" + qstrAppName.toLower();
}


QString IComUIMainWinBase::GetForumAddress()
{
    return "https://www.inviska.com/forum/";
}


QString IComUIMainWinBase::GetChangelogAddress()
{
    return GetWebsite() + "/changelog.html";
}

