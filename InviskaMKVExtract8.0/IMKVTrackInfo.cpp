#include <QtWidgets>
#include "IMKVTrackInfo.h"
#include "IMKVCodecLookup.h"
#include "IUIExtract.h"


IMKVTrackInfo::IMKVTrackInfo()
{
    Reset();
}


void IMKVTrackInfo::Reset()
{
    m_iTrackID = -1;

    m_iTrackType = IUIExtract::Invalid;

    m_qstrTrackType.clear();
    m_qstrName.clear();
    m_qstrLanguage.clear();
    m_qstrCodec.clear();

    m_bSupported = false;
}


int IMKVTrackInfo::GenerateTrackDescription(IMKVFileInfo & rfinfoFileInfo, QString & rqstrDescription)
{
    if (m_qstrName.isNull())
        m_qstrName = tr("Unnamed", "Used when a track has no name");

    rfinfoFileInfo.UpdateTrackCounts(m_iTrackType);

    rqstrDescription = tr("%1 %2 - Name: %3 | Language: %4 | Codec: %5").arg(m_qstrTrackType).arg(rfinfoFileInfo.GetTrackCountForType(m_iTrackType)).arg(m_qstrName).arg(m_qstrLanguage).arg(m_qstrCodec);
    return m_iTrackType;
}


void IMKVTrackInfo::ReadTrackInfo(QJsonObject qjobjTrackObject)
{
    QJsonObject qjobjTrackProperties = qjobjTrackObject.value("properties").toObject();
    SetTrackType(qjobjTrackObject.value("type").toString());

    m_iTrackID      = qjobjTrackObject.value("id").toInt();
    m_qstrCodec     = qjobjTrackObject.value("codec").toString();
    m_qstrCodecID   = qjobjTrackProperties.value("codec_id").toString();
    m_qstrLanguage  = qjobjTrackProperties.value("language").toString();
    m_qstrName      = qjobjTrackProperties.value("track_name").toString();

    m_bSupported    = IMKVCodecLookup::CodecSupported(m_qstrCodecID);
}


void IMKVTrackInfo::ReadTrackInfo(const QString & krqstrTrackLine)
{
    // Track ID 0:
    int iStart = 9;
    int iNumDigits = 0;
    while (krqstrTrackLine.at(iStart+iNumDigits).isDigit())
        ++iNumDigits;
    m_iTrackID = krqstrTrackLine.mid(iStart, iNumDigits).toInt();

    iStart = krqstrTrackLine.indexOf(' ', iStart) + 1;
    int iNumChar = krqstrTrackLine.indexOf(' ', iStart) - iStart;
    SetTrackType(krqstrTrackLine.mid(iStart, iNumChar));

    iStart = krqstrTrackLine.indexOf('(', iStart+iNumChar) + 1;
    iNumChar = krqstrTrackLine.indexOf(')', iStart) - iStart;
    m_qstrCodec = krqstrTrackLine.mid(iStart, iNumChar);

    // codec_id:A_FLAC
    iStart = krqstrTrackLine.indexOf("codec_id:", iStart+iNumChar) + 9;
    iNumChar = krqstrTrackLine.indexOf(' ', iStart) - iStart;
    m_qstrCodecID = krqstrTrackLine.mid(iStart, iNumChar);

    // language:und
    iStart = krqstrTrackLine.indexOf("language:", iStart+iNumChar) + 9;
    iNumChar = krqstrTrackLine.indexOf(' ', iStart) - iStart;
    m_qstrLanguage = krqstrTrackLine.mid(iStart, iNumChar);

    // track_name:Styled\sSubs (\s indicates spaces.  Will not be present if name not set)
    iStart = krqstrTrackLine.indexOf("track_name:", iStart+iNumChar);
    if (iStart != -1)
    {
        iStart += 11;
        iNumChar = krqstrTrackLine.indexOf(' ', iStart) - iStart;
        m_qstrName = krqstrTrackLine.mid(iStart, iNumChar);
        m_qstrName.replace("\\s", " ", Qt::CaseInsensitive);
    }

    m_bSupported  = IMKVCodecLookup::CodecSupported(m_qstrCodecID);
}


void IMKVTrackInfo::SetTrackType(const QString & krqstrTrackType)
{   
    if (krqstrTrackType == "video")
    {
        m_iTrackType = IUIExtract::Video;
        m_qstrTrackType = tr("Video");
    }
    else if (krqstrTrackType == "audio")
    {
        m_iTrackType = IUIExtract::Audio;
        m_qstrTrackType = tr("Audio");
    }
    else if (krqstrTrackType == "subtitles")
    {
        m_iTrackType = IUIExtract::Subtitles;
        m_qstrTrackType = tr("Subtitles");
    }
}
