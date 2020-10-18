#include <QApplication>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QProcessEnvironment>
#include "IUIMainWindow.h"
#include "IComSysSingleInstance.h"

// Returns the directory in which to store
QString GetSettingsFilePath(const QString & krqstrAppName);

// Returns true if the program is installed, so we know wether to run in install or portable mode
bool ProgramInstalled(const QString & krqstrAppDirPath);


int main(int argc, char* argv[])
{
    // GTK+ style doesn't work on Unity, so set style to Fusion if desktop environment is Unity
    if (QProcessEnvironment::systemEnvironment().value("XDG_CURRENT_DESKTOP").startsWith("Unity"))
        QApplication::setStyle("Fusion");

    QString qstrAppName        = IUIMainWindow::GetAppName();
    QString qstrAppNameNoSpace = IUIMainWindow::GetAppNameNoSpaces();

    QCoreApplication::setOrganizationName("Inviska");
    QCoreApplication::setOrganizationDomain("inviska.com");
    QCoreApplication::setApplicationName(qstrAppName);
    QCoreApplication::setApplicationVersion(IUIMainWindow::GetVersion());
    QApplication qaApplication(argc, argv);

    QSettings qsetSettings(GetSettingsFilePath(qstrAppNameNoSpace), QSettings::IniFormat);
    IComSysSingleInstance snglSingleInstance(qstrAppNameNoSpace);
    if (qsetSettings.value("Application/SingleInstanceOnly", false).toBool())
    {
        if (snglSingleInstance.TryToRun() == false)
            return 0;
    }

    IUIMainWindow mwMainWin(qsetSettings, snglSingleInstance);
    mwMainWin.show();

    return qaApplication.exec();
}


QString GetSettingsFilePath(const QString & krqstrAppName)
{
    QString qstrIniFileName = krqstrAppName + ".ini";
    QString qstrAppDirPath = QCoreApplication::applicationDirPath();

    if (ProgramInstalled(qstrAppDirPath))
    {
        // On Windows AppConfigLocation returns %USERPROFILE%\AppData\Local.  I expected it to return Roaming?  Used AppDataLocation instead since that returns Roaming.
        #ifdef Q_OS_WIN
        QString qstrIniFilePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);    // %USERPROFILE%\AppData\Roaming\Inviska
        #else
        QString qstrIniFilePath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);  // Linux: $HOME/.config/Inviska   Mac: $HOME/Library/Preferences/Inviska
        #endif
        return qstrIniFilePath.left(qstrIniFilePath.lastIndexOf('/')) + '/' + qstrIniFileName;
    }
    return qstrAppDirPath + '/' + qstrIniFileName;
}


bool ProgramInstalled(const QString & krqstrAppDirPath)
{
    // Whether to run in portable mode (settings stored in exe directory) or install mode (settings stored in appropriate place in user directory) is determined by the install directory
    #if   defined(Q_OS_WIN) || defined(Q_OS_MACOS)
    // On Windows and Mac the installer adds an Install.sys file to the install directory (.app/Contents/MacOS on Mac).  If Install.sys present the program should run in install mode, other wise it should be portable
    return QFileInfo::exists(krqstrAppDirPath + "/Install.sys");
    #else
    // On Linux we check if the executable is installed to /usr/bin/.  If it is the program should run in install mode, other wise it should be portable
    return (krqstrAppDirPath == "/usr/bin");
    #endif
}
