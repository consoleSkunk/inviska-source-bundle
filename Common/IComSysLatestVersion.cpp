#include <QtWidgets>
#include "IComSysLatestVersion.h"
#include "IComSysAbsoluteDay.h"
#include "IUIMainWindow.h"


IComSysLatestVersion::IComSysLatestVersion(IUIMainWindow* pmwMainWindow) : m_rqsetSettings(pmwMainWindow->GetSettings())
{
    m_pmwMainWindow = pmwMainWindow;
    m_iMessageID = 0;
    m_iError = QNetworkReply::NoError;

    // Having a problem on Linux where I get an SSL error even though SSL isn't used.
    // It seems I'm not the only one - http://stackoverflow.com/questions/26361145/qsslsocket-error-when-ssl-is-not-used
    // Copiling Qt with -no-openssl didn't resolve the issue, so I'll just I'll just hide the messages since it doesn't appear to cause any functional issues.
    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");

    if (m_rqsetSettings.value("VersionCheck/CheckForNewVersion", true).toBool())
        CheckForNewVersion();
}


void IComSysLatestVersion::CheckForNewVersion()
{
    if (LastCheckWasToday())
    {
        LoadInfoFromSettings();
    }
    else
    {
        m_pqnamNetworkMan = new QNetworkAccessManager(m_pmwMainWindow);
        m_pqnrReply = m_pqnamNetworkMan->get(QNetworkRequest(QUrl("http://www.inviska.com/versioninfo/LatestVersion")));
        connect(m_pqnrReply, SIGNAL(finished()),                         this, SLOT(Finished()));
        connect(m_pqnrReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(Error(QNetworkReply::NetworkError)));
    }
}


bool IComSysLatestVersion::LastCheckWasToday()
{
    QDate qdatCurrentDate = QDate::currentDate();
    int iCurrentAbsDay = IComSysAbsoluteDay::ConvertDateToAbsoluteDay(qdatCurrentDate.day(), qdatCurrentDate.month(), qdatCurrentDate.year());
    int iLastCheckAbsDay = m_rqsetSettings.value("VersionCheck/LastCheckAbsDay", 0).toInt();
    return iLastCheckAbsDay == iCurrentAbsDay;
}


void IComSysLatestVersion::LoadInfoFromSettings()
{
    m_rqsetSettings.beginGroup("VersionCheck");
    m_qstrLatestVersion = m_rqsetSettings.value("LatestVersion", "").toString();
    m_iMessageID        = m_rqsetSettings.value("LastMessageID", 0).toInt();
    m_qstrMessageTitle  = m_rqsetSettings.value("LastMessageTitle", "").toString();
    m_qstrMessageBody   = m_rqsetSettings.value("LastMessageBody", "").toString();
    m_rqsetSettings.endGroup();
}


void IComSysLatestVersion::SaveInfoToSettings()
{
    QDate qdatCurrentDate = QDate::currentDate();
    int iCurrentAbsDay = IComSysAbsoluteDay::ConvertDateToAbsoluteDay(qdatCurrentDate.day(), qdatCurrentDate.month(), qdatCurrentDate.year());

    m_rqsetSettings.beginGroup("VersionCheck");
    m_rqsetSettings.setValue("LastCheckAbsDay", iCurrentAbsDay);
    m_rqsetSettings.setValue("LatestVersion", m_qstrLatestVersion);
    m_rqsetSettings.setValue("LastMessageID", m_iMessageID);
    m_rqsetSettings.setValue("LastMessageTitle", m_qstrMessageTitle);
    m_rqsetSettings.setValue("LastMessageBody", m_qstrMessageBody);
    m_rqsetSettings.endGroup();
}


void IComSysLatestVersion::Finished()
{
    QByteArray qbaData = m_pqnrReply->readAll();
    if (qbaData.isEmpty() == false && m_iError == QNetworkReply::NoError)
    {
        QString qstrVersionInfo = QString::fromUtf8(qbaData);
        ProcessVersionInformation(qstrVersionInfo);
    }
    else
    {
        if (LastCheckWasToday())
            LoadInfoFromSettings();
    }

    m_pqnrReply->deleteLater();
    m_pqnamNetworkMan->deleteLater();
}


void IComSysLatestVersion::Error(const QNetworkReply::NetworkError kqnreErrorCode)
{
    m_iError = kqnreErrorCode;
}


void IComSysLatestVersion::ProcessVersionInformation(QString & rqstrVersionInfo)
{
    #ifdef Q_OS_WIN
    QString qstrLatestVerPlatform = "[LatestVersionWin]";
    #elif defined Q_OS_MACOS
    QString qstrLatestVerPlatform = "[LatestVersionMac]";
    #else
    QString qstrLatestVerPlatform = "[LatestVersionLinux]";
    #endif

    QStringList qstrlVersionInfo = rqstrVersionInfo.split('\n');
    QStringList::const_iterator kitLine = qstrlVersionInfo.constBegin();
    while (kitLine != qstrlVersionInfo.constEnd())
    {
        if (kitLine->startsWith(qstrLatestVerPlatform))
            ReadVersionSection(qstrlVersionInfo, ++kitLine);
        else if (kitLine->startsWith("[Message]"))
            ReadMessageSection(qstrlVersionInfo, ++kitLine);
        else
            ++kitLine;
    }

    DisplayMessage();
    DisplayVersionUpdate();

    SaveInfoToSettings();
}


void IComSysLatestVersion::ReadVersionSection(const QStringList & krqstrlVersionInfo, QStringList::const_iterator & rkitLine)
{
    QString qstrAppNameNoSpaces = m_pmwMainWindow->GetAppNameNoSpaces();
    while (rkitLine != krqstrlVersionInfo.constEnd() && rkitLine->startsWith('[') == false)
    {
        if (rkitLine->startsWith(qstrAppNameNoSpaces))
            m_qstrLatestVersion = GetValue(*rkitLine);
        ++rkitLine;
    }
}


void IComSysLatestVersion::ReadMessageSection(const QStringList & krqstrlVersionInfo, QStringList::const_iterator & rkitLine)
{
    while (rkitLine != krqstrlVersionInfo.constEnd() && rkitLine->startsWith('[') == false)
    {
        if (rkitLine->startsWith("ID="))
            m_iMessageID = GetValue(*rkitLine).toInt();
        else if (rkitLine->startsWith("MessageForApp="))
            m_qstrMessageForApp = GetValue(*rkitLine);
        else if (rkitLine->startsWith("MessageForPlatform="))
            m_qstrMessageForPlatform = GetValue(*rkitLine);
        else if (rkitLine->startsWith("Title="))
            m_qstrMessageTitle = GetValue(*rkitLine);
        else if (rkitLine->startsWith("Body="))
            m_qstrMessageBody = GetValue(*rkitLine);
        ++rkitLine;
    }

    #ifdef Q_OS_WIN
    QString qstrPlatform = "Windows";
    #elif defined Q_OS_MACOS
    QString qstrPlatform = "Mac";
    #else
    QString qstrPlatform = "Linux";
    #endif

    if (((m_qstrMessageForApp      == "All" || m_qstrMessageForApp      == m_pmwMainWindow->GetAppNameNoSpaces()) &&
         (m_qstrMessageForPlatform == "All" || m_qstrMessageForPlatform == qstrPlatform)) == false)
    {
        m_iMessageID = 0;
        m_qstrMessageTitle.clear();
        m_qstrMessageBody.clear();
    }
}


QString IComSysLatestVersion::GetValue(const QString & krqstrLine)
{
    int iStart = krqstrLine.indexOf('=')+1;
    return krqstrLine.mid(iStart, krqstrLine.lastIndexOf(';')-iStart);
}


bool IComSysLatestVersion::DisplayMessage()
{   
    QSettings & rqsetSettings = m_pmwMainWindow->GetSettings();
    int iLastMessageID = rqsetSettings.value("VersionCheck/LastMessageID", 0).toInt();
    if (m_iMessageID > iLastMessageID && m_qstrMessageBody.isEmpty() == false)
    {
        QString qstrMessage = "<center><h1>" + m_qstrMessageTitle + "</h1>" + m_qstrMessageBody + "</center>";
        QMessageBox qmbMessage(QMessageBox::NoIcon, m_qstrMessageTitle, qstrMessage, QMessageBox::Ok, m_pmwMainWindow);
        qmbMessage.resize(qmbMessage.sizeHint());
        qmbMessage.exec();
        return true;
    }
    return false;
}


void IComSysLatestVersion::DisplayVersionUpdate()
{
    if (m_rqsetSettings.value("VersionCheck/NotifyWhenNewVersion", true).toBool() == false)
        return;

    QVersionNumber qvnCurrent       = QVersionNumber::fromString(m_pmwMainWindow->GetVersion());
    QVersionNumber qvnLatest        = QVersionNumber::fromString(m_qstrLatestVersion);
    QVersionNumber qvnStoredLatest  = QVersionNumber::fromString(m_rqsetSettings.value("VersionCheck/LatestVersion", "0").toString());

    if (qvnLatest > qvnCurrent && qvnLatest > qvnStoredLatest)
    {
        QDialog* pqdlgNewVersion = new QDialog(m_pmwMainWindow);
        pqdlgNewVersion->setWindowTitle(tr("New Version Available"));
        pqdlgNewVersion->setModal(true);
        pqdlgNewVersion->setAttribute(Qt::WA_DeleteOnClose);
        pqdlgNewVersion->setWindowFlags(pqdlgNewVersion->windowFlags() & ~Qt::WindowContextHelpButtonHint);

        QPixmap qpixmTitleImage(":/Resources/HelpAboutBanner.png");
        QLabel* pqlblTitleBanner = new QLabel;
        pqlblTitleBanner->setPixmap(qpixmTitleImage);
        pqlblTitleBanner->setAlignment(Qt::AlignTop | Qt::AlignHCenter);


        QString qstrMessage = tr("<center><p><h1>New Version Available</h1></p>"
                                 "<p>Current version: %2<br>Latest version: %3</p>"
                                 "</p>You can download the new version from:</p>"
                                 "<p><a href='%4'>%4</a></p>"
                                 "<p>A list of changes can be seen in the <a href='%5'>changelog</a>.</p>"
                                 "</center>")
                                 .arg(m_pmwMainWindow->GetVersion())
                                 .arg(m_qstrLatestVersion)
                                 .arg(m_pmwMainWindow->GetWebsite())
                                 .arg(m_pmwMainWindow->GetChangelogAddress());

        QLabel* pqlblMessage = new QLabel(qstrMessage);
        pqlblMessage->setTextFormat(Qt::RichText);
        pqlblMessage->setOpenExternalLinks(true);

        QDialogButtonBox* pqdbbOK = new QDialogButtonBox(QDialogButtonBox::Ok);
        pqdbbOK->setOrientation(Qt::Horizontal);
        connect(pqdbbOK, SIGNAL(accepted()), pqdlgNewVersion, SLOT(accept()));

        QVBoxLayout* pqvblLayout = new QVBoxLayout;
        pqvblLayout->addWidget(pqlblTitleBanner);
        pqvblLayout->addWidget(pqlblMessage);
        pqvblLayout->addStretch();
        pqvblLayout->addWidget(pqdbbOK);

        pqdlgNewVersion->setLayout(pqvblLayout);
        pqdlgNewVersion->resize(pqdlgNewVersion->sizeHint());
        pqdlgNewVersion->exec();
    }
}


bool IComSysLatestVersion::MessageAvailable()
{
    return m_iMessageID > 0 && m_qstrMessageTitle.isEmpty() == false && m_qstrMessageBody.isEmpty() == false;
}
