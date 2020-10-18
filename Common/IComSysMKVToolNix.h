#ifndef IComSysMKVToolNix_h
#define IComSysMKVToolNix_h

#include <QObject>
#include <QSettings>
#include "IComSysMKVToolNixVer.h"
class IUIMainWindow;


class IComSysMKVToolNix : public QObject
{
    Q_OBJECT

private:
    // Pointer to main window to get settings and parent for dialog
    IUIMainWindow*          m_pmwMainWindow;

    // Class for getting MKVToolNix version
    IComSysMKVToolNixVer    m_mkvvMKVToolNixVer;

    // Contains name of MKVToolNix executable files used by the program
    QString                 m_qstrMKVInfo;
    QString                 m_qstrMKVMerge;
    QString                 m_qstrMKVExtract;

    // Path of MKVToolNix directory
    QString                 m_qstrMKVToolNixPath;

    // Version of MKVToolNix installed
    int                     m_iMKVToolNixMajorVer;
    int                     m_iMKVToolNixMinorVer;
    enum                    MKVToolNixVErsionInvalid {VerStartupVal = -1};

public:
    IComSysMKVToolNix(IUIMainWindow* pmwMainWindow);

private slots:
    // Attempts to locate MKVToolNix and notifies user if not present
    bool LocateMKVToolNix();

private:
    // Functions to locate MKVToolNix on different operating systems
    bool LocateMKVToolNixWin();
    bool LocateMKVToolNixMac();
    bool LocateMKVToolNixLinux();

private:
    // Checks various directories to see if MKVToolNix is present
    bool LocateMKVToolNixCheckDirectories();

public:
    // Displays message saying that MKVToolNix is required
    void ShowMsgRequiresMKVToolNix();

    // Strips off trailing / and \ from the string and replaces seperators with /
    void ProcessMKVToolNixPath(QString & rqstrMKVToolNixPath);

    // Returns true of MKVToolNix is at the path stored in m_qstrMKVToolNixPath, otherwise returns false
    bool ValidateMKVToolNixPath(const QString & krqstrMKVToolNixPath);

    // Returns true if we have a valid stored MKVToolNix path
    bool StoredMKVToolNixPathValid();

    // Sets MKVToolNix path to passed string (no validation check performed)
    void SetMKVToolNixPath(const QString & krqstrPath);

    // Saves the MKVToolinix location once it has been found
    void SaveMKVToolNixLocation();

    // Called to initiate proccess to determine the MKVToolNix version
    void DetermineMKVToolNixVersion();

    // Sets the MKVToolNix version and initiates processing of command line parameters
    void SetMKVToolNixVersion(const int kiMajorVersion, const int kiMinorVersion);

    // Indicates if the installed version of MKVToolNix supports the new MKVExtract interface
    bool NewMKVExtractInterface();

    // Indicates if the installed version of MKVToolNix supports the output of Identify information in JSON format
    bool IdentifyJSONAvailable();

    // Indicates if the installed version of MKVToolNix requires the Mac output to file workaround to be used
    bool MacWorkaroundRequired();

    // Returns major version of MKVToolNix
    int GetMKVToolNixMajorVer() {return m_iMKVToolNixMajorVer;}

    // Returns path for MKVToolNix executables, or an empty if the path is not set
    QString GetMKVInfoPath();
    QString GetMKVMergePath();
    QString GetMKVExtractPath();
    QString GetMKVToolNixPath();
};

#endif // IComSysMKVToolNix_h

