#ifndef IMKVAttachmentInfo_h
#define IMKVAttachmentInfo_h

#include <QString>
#include <QJsonObject>
class IMKVFileInfo;


class IMKVAttachmentInfo
{
    Q_DECLARE_TR_FUNCTIONS(IMKVAttachmentInfo);

private:
    // Attachment ID for extraction by MKVExtract
    int                         m_iAttachmentID;

    // Attachment file name
    QString                     m_qstrFileName;

public:
    IMKVAttachmentInfo();

    // Resets state ready to read the next attachment information
    void Reset();

    // Updates the file information and sets the passed string to the attachment filename
    void GenerateAttatchmentDescription(IMKVFileInfo & rfinfoFileInfo, QString & rqstrDescription);

    // Read attachment information from passed JSON object
    void ReadAttachmentInfo(QJsonObject qjobjAttachmentObject);

    // Read attachment information from passed Identify verbose chapter line
    void ReadAttachmentInfo(const QString & krqstrTrackLine);

    // Accessor Functions
    int GetAttachmentID() {return m_iAttachmentID;}
};

#endif // IMKVAttachmentInfo_h
