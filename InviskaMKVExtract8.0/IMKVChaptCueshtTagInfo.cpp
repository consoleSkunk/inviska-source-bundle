#include <QtWidgets>
#include "IMKVChaptCueshtTagInfo.h"
#include "IUIExtract.h"


IMKVChaptCueshtTagInfo::IMKVChaptCueshtTagInfo()
{
    Reset();
}


void IMKVChaptCueshtTagInfo::Reset()
{
    m_iChapterCount  = 0;
    m_iCueTrackCount = 0;
    m_iTagCount      = 0;
}


int IMKVChaptCueshtTagInfo::GenerateChaptersDescription(IMKVFileInfo & rfinfoFileInfo, QString & rqstrDescription)
{
    rfinfoFileInfo.UpdateTrackCounts(IUIExtract::Chapters);

    rqstrDescription = tr("Chapters - Number of Entries: %1").arg(m_iChapterCount);
    return m_iChapterCount;
}


int IMKVChaptCueshtTagInfo::GenerateCuesheetDescription(IMKVFileInfo & rfinfoFileInfo, QString & rqstrDescription)
{
    rfinfoFileInfo.UpdateTrackCounts(IUIExtract::Cuesheet);

    rqstrDescription = tr("Cuesheet - Number of Tracks: %1").arg(m_iCueTrackCount);
    return m_iCueTrackCount;
}


int IMKVChaptCueshtTagInfo::GenerateTagsDescription(IMKVFileInfo & rfinfoFileInfo, QString & rqstrDescription)
{
    rfinfoFileInfo.UpdateTrackCounts(IUIExtract::Tags);

    rqstrDescription = tr("Tags - Number of Entries: %1").arg(m_iTagCount);
    return m_iTagCount;

}


bool IMKVChaptCueshtTagInfo::ReadChapterInfo(QJsonObject qjobjFileObject)
{
    QJsonArray qjrgChapters = qjobjFileObject.value("chapters").toArray();

    if (qjrgChapters.size() != 0)
        m_iChapterCount = qjrgChapters.at(0).toObject().value("num_entries").toInt();

    return (m_iChapterCount != 0);
}


void IMKVChaptCueshtTagInfo::ReadChapterInfo(const QString & krqstrChapterLine)
{
    // Chapters: 5 entries
    int iStart = 10;
    int iNumDigits = 0;
    while (krqstrChapterLine.at(iStart+iNumDigits).isDigit())
        ++iNumDigits;

    m_iChapterCount = krqstrChapterLine.mid(iStart, iNumDigits).toInt();
}


bool IMKVChaptCueshtTagInfo::ProcessCuesheet(QTextStream & rqtsrmCuesheetStream)
{
    int iIndex = 0;
    QString strLine = rqtsrmCuesheetStream.readLine();
    while (!strLine.isNull())
    {
        /*if (strLine.startsWith("  TRACK"))
            ++m_iCueTrackCount;*/

        iIndex = 0;
        while (strLine.at(iIndex) == ' ')
            ++iIndex;

        if (strLine.mid(iIndex).startsWith("TRACK"))
            ++m_iCueTrackCount;        

        strLine = rqtsrmCuesheetStream.readLine();
    }

    return (m_iCueTrackCount != 0);
}


bool IMKVChaptCueshtTagInfo::ProcessTags(QTextStream & rqtsrmTagsFileStream)
{
    QString strLine = rqtsrmTagsFileStream.readLine();
    while (!strLine.isNull())
    {
        if (strLine.contains("<Tag>"))
            ++m_iTagCount;

        strLine = rqtsrmTagsFileStream.readLine();
    }

    return (m_iTagCount != 0);
}
