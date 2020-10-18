#ifndef IComSysLatestVersion_h
#define IComSysLatestVersion_h

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
class IUIMainWindow;


class IComSysLatestVersion : public QObject
{
    Q_OBJECT

private:
    // Main window
    IUIMainWindow*                  m_pmwMainWindow;

    // Application settings
    QSettings &                     m_rqsetSettings;

    // Network access manager for downloading version file
    QNetworkAccessManager*          m_pqnamNetworkMan;

    // Network reply to which data is saved ready for reading by application
    QNetworkReply*                  m_pqnrReply;

    // Records error number if one occured
    int                             m_iError;

    // Stores the latest version of the software
    QString                        m_qstrLatestVersion;

    // Stores message information
    // MessageForApp should be GetAppNameNoSpaces() or "All"
    // MessageForPlatform should be "Windows" "Mac" "Linux" or "All"
    int                             m_iMessageID;
    QString                         m_qstrMessageForApp;
    QString                         m_qstrMessageForPlatform;
    QString                         m_qstrMessageTitle;
    QString                         m_qstrMessageBody;

public:
    IComSysLatestVersion(IUIMainWindow* pmwMainWindow);

private:
    // Checks file on website to determine latest vesrion of softare
    void CheckForNewVersion();

    // Returns true if the last new version check was today (to prevent checking multiple times per day)
    bool LastCheckWasToday();

    // Called if the last checkw as today, this loads the information sent
    void LoadInfoFromSettings();

    // Saves last check date, version info and message info
    void SaveInfoToSettings();

private slots:
    // Reads all data from reply and initialises processing of data
    void Finished();

    // Records if error occured
    void Error(const QNetworkReply::NetworkError kqnreErrorCode);

private:
    // Processes contents of version information file
    void ProcessVersionInformation(QString & rqstrVersionInfo);
    void ReadVersionSection(const QStringList & krqstrlVersionInfo, QStringList::const_iterator & rkitLine);
    void ReadMessageSection(const QStringList & krqstrlVersionInfo, QStringList::const_iterator & rkitLine);

    // Returns the value from a specific line (all the text between the first '=' and the last ';')
    QString GetValue(const QString & krqstrLine);

    // Displays a message if one is present, returning true if one was displayed
    bool DisplayMessage();

    // Displays a message notifying user of a new version
    void DisplayVersionUpdate();

public:
    // Returns true if there is a message to display;
    bool MessageAvailable();

    // Accessor functions
    QString   GetLatestVersion()    {return m_qstrLatestVersion.isEmpty() ? tr("Unknown", "Used when latest version can't be read") : m_qstrLatestVersion;}
    int       GetMessageID()        {return m_iMessageID;}
    QString & GetMessageTitle()     {return m_qstrMessageTitle;}
    QString & GetMessageBody()      {return m_qstrMessageBody;}
};

#endif // IComSysLatestVersion_h
