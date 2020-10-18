#include <QCoreApplication>
#include <QtWidgets>
#include <QDir>
#include "IUIMainWindow.h"
#include "IDlgPreferences.h"
#include "IComSysMKVToolNix.h"


IComSysMKVToolNix::IComSysMKVToolNix(IUIMainWindow* pmwMainWindow) : QObject(pmwMainWindow),
                                                                     m_mkvvMKVToolNixVer(pmwMainWindow, this)
{
    m_pmwMainWindow       = pmwMainWindow;
    m_iMKVToolNixMajorVer = VerStartupVal;
    m_iMKVToolNixMinorVer = VerStartupVal;

    #ifdef Q_OS_WIN
    m_qstrMKVInfo    = "mkvinfo.exe";
    m_qstrMKVMerge   = "mkvmerge.exe";
    m_qstrMKVExtract = "mkvextract.exe";
    #else
    m_qstrMKVInfo    = "mkvinfo";
    m_qstrMKVMerge   = "mkvmerge";
    m_qstrMKVExtract = "mkvextract";
    #endif

    QTimer::singleShot(0, this, SLOT(LocateMKVToolNix()));
}


bool IComSysMKVToolNix::LocateMKVToolNix()
{
    if (LocateMKVToolNixCheckDirectories() == true)
    {
        DetermineMKVToolNixVersion();
        return true;
    }

    ShowMsgRequiresMKVToolNix();

    if (LocateMKVToolNixCheckDirectories() == true)
    {
        DetermineMKVToolNixVersion();
        return true;
    }
    return false;
}


bool IComSysMKVToolNix::LocateMKVToolNixCheckDirectories()
{     
    m_qstrMKVToolNixPath = m_pmwMainWindow->GetSettings().value("MKVToolNix/MKVToolNixPath", QString()).toString();
    ProcessMKVToolNixPath(m_qstrMKVToolNixPath);
    if (ValidateMKVToolNixPath(m_qstrMKVToolNixPath) == true)
    {
        return true;
    }

    m_qstrMKVToolNixPath = QDir::fromNativeSeparators(QCoreApplication::applicationDirPath());
    if (ValidateMKVToolNixPath(m_qstrMKVToolNixPath) == true)
    {
        SaveMKVToolNixLocation();
        return true;
    }

    #if   defined(Q_OS_WIN)
    return LocateMKVToolNixWin();
    #elif defined(Q_OS_MACOS)
    return LocateMKVToolNixMac();
    #else
    return LocateMKVToolNixLinux();
    #endif
}


bool IComSysMKVToolNix::LocateMKVToolNixWin()
{
    QSettings qsetMKVToolNixGUI("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\MKVToolNix-gui.exe", QSettings::NativeFormat);
    m_qstrMKVToolNixPath = QDir::fromNativeSeparators(qsetMKVToolNixGUI.value("Default", "").toString());
    m_qstrMKVToolNixPath = m_qstrMKVToolNixPath.left(m_qstrMKVToolNixPath.lastIndexOf('/'));
    if (ValidateMKVToolNixPath(m_qstrMKVToolNixPath) == true)
    {
        SaveMKVToolNixLocation();
        return true;
    }

    QSettings qsetMMG("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\mmg.exe", QSettings::NativeFormat);
    m_qstrMKVToolNixPath = QDir::fromNativeSeparators(qsetMMG.value("Default", "").toString());
    m_qstrMKVToolNixPath = m_qstrMKVToolNixPath.left(m_qstrMKVToolNixPath.lastIndexOf('/'));
    if (ValidateMKVToolNixPath(m_qstrMKVToolNixPath) == true)
    {
        SaveMKVToolNixLocation();
        return true;
    }

    m_qstrMKVToolNixPath.clear();
    return false;
}


bool IComSysMKVToolNix::LocateMKVToolNixMac()
{
    // We prioritise the version in /Applications because it's possible the version in /usr/local/bin is an older version
    QDir qdirAppDirReader;
    qdirAppDirReader.setPath(QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation).at(1));
    qdirAppDirReader.setFilter(QDir::Dirs);
    qdirAppDirReader.setSorting(QDir::Name | QDir::Reversed);
    QFileInfoList qfilDirList = qdirAppDirReader.entryInfoList();

    // Multiple version of MKVToolNix can be insalled in /Applications get the start and end index of all MKVToolNix installations
    int iFirstIndex = -1;
    int iLastIndex = -1;
    for (int iIndex = 0 ; iIndex < qfilDirList.size() ; ++iIndex)
    {
        if (qfilDirList.at(iIndex).fileName().startsWith("MKVToolNix-"))
        {
            if (iFirstIndex == -1)
                iFirstIndex = iLastIndex = iIndex;
            else
                iLastIndex = iIndex;
        }
        else if (iLastIndex != -1)
        {
            break;
        }
    }

    if (iFirstIndex != -1)
    {
        int iHighestVerIndex = iFirstIndex;

        // Multiple versions installed so find newest version
        if (iFirstIndex != iLastIndex)
        {
            QString qstrName;
            QVersionNumber qvnHighest, qvnCurrent;
            for (int iIndex = iFirstIndex ; iIndex <= iLastIndex ; ++iIndex)
            {
                qstrName = qfilDirList.at(iIndex).fileName();
                qvnCurrent = QVersionNumber::fromString(qstrName.mid(qstrName.indexOf('-') + 1));
                if (qvnCurrent > qvnHighest)
                {
                    qvnHighest = qvnCurrent;
                    iHighestVerIndex = iIndex;
                }
            }
        }

        m_qstrMKVToolNixPath = qfilDirList.at(iHighestVerIndex).absoluteFilePath() + "/Contents/MacOS";
        if (ValidateMKVToolNixPath(m_qstrMKVToolNixPath) == true)
        {
            SaveMKVToolNixLocation();
            return true;
        }
    }

    // The README.MacOS.txt file that comes with MKVToonix says to copy the binaries to /usr/local/bin
    m_qstrMKVToolNixPath = "/usr/local/bin";
    if (ValidateMKVToolNixPath(m_qstrMKVToolNixPath) == true)
    {
        SaveMKVToolNixLocation();
        return true;
    }

    // MacOS 10.11 introduced "System Integrity Protection" which stops you writing to /usr/bin
    // However, it's possible that users of earlier versions could have put the MKVToolNix binaries in /usr/bin
    m_qstrMKVToolNixPath = "/usr/bin";
    if (ValidateMKVToolNixPath(m_qstrMKVToolNixPath) == true)
    {
        SaveMKVToolNixLocation();
        return true;
    }

    m_qstrMKVToolNixPath.clear();
    return false;
}



bool IComSysMKVToolNix::LocateMKVToolNixLinux()
{
    m_qstrMKVToolNixPath = "/usr/bin";
    if (ValidateMKVToolNixPath(m_qstrMKVToolNixPath) == true)
    {
        SaveMKVToolNixLocation();
        return true;
    }

    m_qstrMKVToolNixPath.clear();
    return false;
}


void IComSysMKVToolNix::ShowMsgRequiresMKVToolNix()
{
    QMessageBox qmbMKVToolNixMsg(m_pmwMainWindow);
    qmbMKVToolNixMsg.setIcon(QMessageBox::Information);
    qmbMKVToolNixMsg.setStandardButtons(QMessageBox::Ok);
    qmbMKVToolNixMsg.setWindowTitle(tr("MKVToolNix Required"));
    qmbMKVToolNixMsg.setTextInteractionFlags(Qt::TextBrowserInteraction);
    qmbMKVToolNixMsg.setText(tr("<p>This program requires MKVToolNix, which can be downloaded from:</p>"
                                "<p><center><a href='https://MKVToolNix.download/downloads.html'>https://MKVToolNix.download/downloads.html</a></center></p>"
                                "<p>After installing MKVToolNix please specify its location in the Preferences, or restart the program to have it auto-detect MKVToolNix.</p>"));
    qmbMKVToolNixMsg.exec();

    new IDlgPreferences(m_pmwMainWindow, IDlgPreferences::MKVToolNixPage);
}


void IComSysMKVToolNix::ProcessMKVToolNixPath(QString & rqstrMKVToolNixPath)
{
    if (rqstrMKVToolNixPath.endsWith('/') || rqstrMKVToolNixPath.endsWith('\\'))
        rqstrMKVToolNixPath.truncate(rqstrMKVToolNixPath.length() - 1);

    if (rqstrMKVToolNixPath.contains('\\'))
        rqstrMKVToolNixPath = QDir::fromNativeSeparators(rqstrMKVToolNixPath);

    #ifdef Q_OS_MACOS
    if (ValidateMKVToolNixPath(rqstrMKVToolNixPath + "/Contents/MacOS"))
        rqstrMKVToolNixPath += "/Contents/MacOS";
    #endif
}


bool IComSysMKVToolNix::ValidateMKVToolNixPath(const QString & krqstrMKVToolNixPath)
{
    if (krqstrMKVToolNixPath.isEmpty())
        return false;

    QString qstrMKVToolNixPath;
    if (krqstrMKVToolNixPath.endsWith('/') || krqstrMKVToolNixPath.endsWith('\\'))
        qstrMKVToolNixPath = QDir::fromNativeSeparators(krqstrMKVToolNixPath.left(krqstrMKVToolNixPath.length() - 1));
    else
        qstrMKVToolNixPath = QDir::fromNativeSeparators(krqstrMKVToolNixPath);

    QString qstrMKVInfoPath     = qstrMKVToolNixPath + '/' + m_qstrMKVInfo;
    QString qstrMKVMergePath    = qstrMKVToolNixPath + '/' + m_qstrMKVMerge;
    QString qstrMKVExtractPath  = qstrMKVToolNixPath + '/' + m_qstrMKVExtract;

    if (QFileInfo::exists(qstrMKVInfoPath) && QFileInfo::exists(qstrMKVMergePath) && QFileInfo::exists(qstrMKVExtractPath))
        return true;

    #ifdef Q_OS_MACOS
    if (qstrMKVToolNixPath.endsWith("/Contents/MacOS") == false)
        return ValidateMKVToolNixPath(qstrMKVToolNixPath + "/Contents/MacOS");
    #endif

    return false;
}


bool IComSysMKVToolNix::StoredMKVToolNixPathValid()
{
    return ValidateMKVToolNixPath(m_qstrMKVToolNixPath);
}


void IComSysMKVToolNix::SetMKVToolNixPath(const QString & krqstrPath)
{
    m_qstrMKVToolNixPath = krqstrPath;
    ProcessMKVToolNixPath(m_qstrMKVToolNixPath);
    SaveMKVToolNixLocation();
}


void IComSysMKVToolNix::SaveMKVToolNixLocation()
{
    m_pmwMainWindow->GetSettings().setValue("MKVToolNix/MKVToolNixPath", m_qstrMKVToolNixPath);
}


void IComSysMKVToolNix::DetermineMKVToolNixVersion()
{
    m_mkvvMKVToolNixVer.DetermineMKVToolNixVersion(GetMKVMergePath());
}


void IComSysMKVToolNix::SetMKVToolNixVersion(const int kiMajorVersion, const int kiMinorVersion)
{
    // qDebug() << tr("MKVToolNix Version: %1.%2").arg(kiMajorVersion).arg(kiMinorVersion);
    // QMessageBox::information(m_pmwMainWindow, tr("MKVToolNix Version"), tr("MKVToolNix Version: %1.%2").arg(kiMajorVersion).arg(kiMinorVersion), QMessageBox::Ok);

    int iVal = m_iMKVToolNixMajorVer;
    m_iMKVToolNixMajorVer = kiMajorVersion;
    m_iMKVToolNixMinorVer = kiMinorVersion;

    if (iVal == VerStartupVal)
        m_pmwMainWindow->ProcessCommandLineParameters();
}


bool IComSysMKVToolNix::NewMKVExtractInterface()
{
    // New MKVExtract interface added in 17.0
    return (m_iMKVToolNixMajorVer >= 17);
}


bool IComSysMKVToolNix::IdentifyJSONAvailable()
{
    // JSON identify added in 8.6
    return (m_iMKVToolNixMajorVer > 8 || (m_iMKVToolNixMajorVer == 8 && m_iMKVToolNixMinorVer >= 6));
}


bool IComSysMKVToolNix::MacWorkaroundRequired()
{
    // Issue began in 14.0
    return (m_iMKVToolNixMajorVer >= 14);
}


QString IComSysMKVToolNix::GetMKVInfoPath()
{
    if (m_qstrMKVToolNixPath.isEmpty())
        return QString();

    return m_qstrMKVToolNixPath + '/' + m_qstrMKVInfo;
}


QString IComSysMKVToolNix::GetMKVMergePath()
{
    if (m_qstrMKVToolNixPath.isEmpty())
        return QString();

    return m_qstrMKVToolNixPath + '/' + m_qstrMKVMerge;
}


QString IComSysMKVToolNix::GetMKVExtractPath()
{
    if (m_qstrMKVToolNixPath.isEmpty())
        return QString();

    return m_qstrMKVToolNixPath + '/' + m_qstrMKVExtract;
}


QString IComSysMKVToolNix::GetMKVToolNixPath()
{
    if (m_qstrMKVToolNixPath.isEmpty())
        return QString();

    return m_qstrMKVToolNixPath;
}
