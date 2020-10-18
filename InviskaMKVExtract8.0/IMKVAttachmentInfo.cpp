#include <QtWidgets>
#include "IMKVAttachmentInfo.h"
#include "IUIExtract.h"


IMKVAttachmentInfo::IMKVAttachmentInfo()
{
    Reset();
}


void IMKVAttachmentInfo::Reset()
{
    m_qstrFileName.clear();

    m_iAttachmentID = -1;
}


void IMKVAttachmentInfo::GenerateAttatchmentDescription(IMKVFileInfo & rfinfoFileInfo, QString & rqstrDescription)
{
    rfinfoFileInfo.UpdateTrackCounts(IUIExtract::Attachment);

    if (m_qstrFileName.isNull())
        rqstrDescription = tr("Unnamed", "Used when an attachment has no name");
    else
        rqstrDescription = m_qstrFileName;
}


void IMKVAttachmentInfo::ReadAttachmentInfo(QJsonObject qjobjAttachmentObject)
{
    m_iAttachmentID = qjobjAttachmentObject.value("id").toInt();
    m_qstrFileName  = qjobjAttachmentObject.value("file_name").toString();
}


void IMKVAttachmentInfo::ReadAttachmentInfo(const QString & krqstrTrackLine)
{
    // Attachment ID 1:
    int iStart = 14;
    int iNumDigits = 0;
    while (krqstrTrackLine.at(iStart+iNumDigits).isDigit())
        ++iNumDigits;
    m_iAttachmentID = krqstrTrackLine.mid(iStart, iNumDigits).toInt();

    // file name 'Agency FB Bold.ttf'
    iStart = krqstrTrackLine.indexOf("file name") + 11;
    int iNumChar = krqstrTrackLine.indexOf('\'', iStart) - iStart;
    m_qstrFileName = krqstrTrackLine.mid(iStart, iNumChar);
}
