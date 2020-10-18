#include <QTableWidgetItem>
#include <QDateTime>
#include "IMetaTagLookup.h"
#include "IUIFileList.h"
#include "IMetaMusic.h"
#include "IMetaExif.h"


IMetaTagLookup::IMetaTagLookup()
{
    PopulateHashFileAttributes();
    PopulateHashMusic();
    PopulateHashExif();
}


void IMetaTagLookup::PopulateHashFileAttributes()
{
    m_qhashTagIdLookupFiAt["createddatetime"]   = FiCreatedDateTime;
    m_qhashTagIdLookupFiAt["createddate"]       = FiCreatedDate;
    m_qhashTagIdLookupFiAt["createdtime"]       = FiCreatedTime;
    m_qhashTagIdLookupFiAt["createddateyyyy"]   = FiCreatedDateYYYY;
    m_qhashTagIdLookupFiAt["createddateyy"]     = FiCreatedDateYY;
    m_qhashTagIdLookupFiAt["createddatemm"]     = FiCreatedDateMM;
    m_qhashTagIdLookupFiAt["createddatedd"]     = FiCreatedDateDD;
    m_qhashTagIdLookupFiAt["createdtimehh"]     = FiCreatedTimeHH;
    m_qhashTagIdLookupFiAt["createdtimemm"]     = FiCreatedTimeMM;
    m_qhashTagIdLookupFiAt["createdtimess"]     = FiCreatedTimeSS;
    m_qhashTagIdLookupFiAt["modifieddatetime"]  = FiModifiedDateTime;
    m_qhashTagIdLookupFiAt["modifieddate"]      = FiModifiedDate;
    m_qhashTagIdLookupFiAt["modifiedtime"]      = FiModifiedTime;
    m_qhashTagIdLookupFiAt["modifieddateyyyy"]  = FiModifiedDateYYYY;
    m_qhashTagIdLookupFiAt["modifieddateyy"]    = FiModifiedDateYY;
    m_qhashTagIdLookupFiAt["modifieddatemm"]    = FiModifiedDateMM;
    m_qhashTagIdLookupFiAt["modifieddatedd"]    = FiModifiedDateDD;
    m_qhashTagIdLookupFiAt["modifiedtimehh"]    = FiModifiedTimeHH;
    m_qhashTagIdLookupFiAt["modifiedtimemm"]    = FiModifiedTimeMM;
    m_qhashTagIdLookupFiAt["modifiedtimess"]    = FiModifiedTimeSS;
}


void IMetaTagLookup::PopulateHashMusic()
{
    m_qhashTagIdLookupMusic["title"]            = MuTitle;
    m_qhashTagIdLookupMusic["artist"]           = MuArtist;
    m_qhashTagIdLookupMusic["album"]            = MuAlbum;
    m_qhashTagIdLookupMusic["track"]            = MuTrack;
    m_qhashTagIdLookupMusic["year"]             = MuYear;
    m_qhashTagIdLookupMusic["genre"]            = MuGenre;
    m_qhashTagIdLookupMusic["comment"]          = MuComment;
    m_qhashTagIdLookupMusic["runtime"]          = MuRunTime;
    m_qhashTagIdLookupMusic["channels"]         = MuChannels;
    m_qhashTagIdLookupMusic["samplerate"]       = MuSampleRate;
    m_qhashTagIdLookupMusic["bitrate"]          = MuBitRate;
}


void IMetaTagLookup::PopulateHashExif()
{
    m_qhashTagIdLookupExif["cameramake"]        = ExCameraMake;
    m_qhashTagIdLookupExif["cameramodel"]       = ExCameraModel;
    m_qhashTagIdLookupExif["fnumber"]           = ExFNumber;
    m_qhashTagIdLookupExif["isospeed"]          = ExISOSpeed;
    m_qhashTagIdLookupExif["exposuretime"]      = ExExposureTime;
    m_qhashTagIdLookupExif["exposuretimedec"]   = ExExposureTimeDec;
    m_qhashTagIdLookupExif["focallength"]       = ExFocalLength;
    m_qhashTagIdLookupExif["program"]           = ExProgram;
    m_qhashTagIdLookupExif["pixeldimx"]         = ExPixelDimX;
    m_qhashTagIdLookupExif["pixeldimy"]         = ExPixelDimY;
    m_qhashTagIdLookupExif["datetime"]          = ExDateTime;
    m_qhashTagIdLookupExif["date"]              = ExDate;
    m_qhashTagIdLookupExif["time"]              = ExTime;
    m_qhashTagIdLookupExif["dateyyyy"]          = ExDateYYYY;
    m_qhashTagIdLookupExif["dateyy"]            = ExDateYY;
    m_qhashTagIdLookupExif["datemm"]            = ExDateMM;
    m_qhashTagIdLookupExif["datedd"]            = ExDateDD;
    m_qhashTagIdLookupExif["timehh"]            = ExTimeHH;
    m_qhashTagIdLookupExif["timemm"]            = ExTimeMM;
    m_qhashTagIdLookupExif["timess"]            = ExTimeSS;
    m_qhashTagIdLookupExif["timesubsec"]        = ExTimeSubSec;
}


QString IMetaTagLookup::GetValueForTagCode(const QTableWidgetItem* kpqtwiFileItem, const int kiTagID)
{
    if (kiTagID == InvalidTag)
        return "";
    if (kiTagID < FiEnd)
        return GetValueForTagCodeFile(kpqtwiFileItem, kiTagID);
    if (kiTagID < MuEnd)
        return GetValueForTagCodeMusic(kpqtwiFileItem, kiTagID);
    if (kiTagID < ExEnd)
        return GetValueForTagCodeExif(kpqtwiFileItem, kiTagID);

    return "";
}


QString IMetaTagLookup::GetValueForTagCodeFile(const QTableWidgetItem* kpqtwiFileItem, const int kiTagID)
{
    if (kpqtwiFileItem->data(IUIFileList::FileInfo).isNull())
        return "";
    const QFileInfo & krqfiFileInfo = kpqtwiFileItem->data(IUIFileList::FileInfo).value<QFileInfo>();

    switch (kiTagID)
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    case FiCreatedDateTime  :   return krqfiFileInfo.birthTime().toString("yyyy-MM-dd HH.mm.ss");
    case FiCreatedDate      :   return krqfiFileInfo.birthTime().date().toString(Qt::ISODate);
    case FiCreatedTime      :   return krqfiFileInfo.birthTime().time().toString("HH.mm.ss");
    case FiCreatedDateYYYY  :   return krqfiFileInfo.birthTime().date().toString("yyyy");
    case FiCreatedDateYY    :   return krqfiFileInfo.birthTime().date().toString("yy");
    case FiCreatedDateMM    :   return krqfiFileInfo.birthTime().date().toString("MM");
    case FiCreatedDateDD    :   return krqfiFileInfo.birthTime().date().toString("dd");
    case FiCreatedTimeHH    :   return krqfiFileInfo.birthTime().time().toString("HH");
    case FiCreatedTimeMM    :   return krqfiFileInfo.birthTime().time().toString("mm");
    case FiCreatedTimeSS    :   return krqfiFileInfo.birthTime().time().toString("ss");
#else
    case FiCreatedDateTime  :   return krqfiFileInfo.created().toString("yyyy-MM-dd HH.mm.ss");
    case FiCreatedDate      :   return krqfiFileInfo.created().date().toString(Qt::ISODate);
    case FiCreatedTime      :   return krqfiFileInfo.created().time().toString("HH.mm.ss");
    case FiCreatedDateYYYY  :   return krqfiFileInfo.created().date().toString("yyyy");
    case FiCreatedDateYY    :   return krqfiFileInfo.created().date().toString("yy");
    case FiCreatedDateMM    :   return krqfiFileInfo.created().date().toString("MM");
    case FiCreatedDateDD    :   return krqfiFileInfo.created().date().toString("dd");
    case FiCreatedTimeHH    :   return krqfiFileInfo.created().time().toString("HH");
    case FiCreatedTimeMM    :   return krqfiFileInfo.created().time().toString("mm");
    case FiCreatedTimeSS    :   return krqfiFileInfo.created().time().toString("ss");
#endif

    case FiModifiedDateTime :   return krqfiFileInfo.lastModified().toString("yyyy-MM-dd HH.mm.ss");
    case FiModifiedDate     :   return krqfiFileInfo.lastModified().date().toString(Qt::ISODate);
    case FiModifiedTime     :   return krqfiFileInfo.lastModified().time().toString("HH.mm.ss");
    case FiModifiedDateYYYY :   return krqfiFileInfo.lastModified().date().toString("yyyy");
    case FiModifiedDateYY   :   return krqfiFileInfo.lastModified().date().toString("yy");
    case FiModifiedDateMM   :   return krqfiFileInfo.lastModified().date().toString("MM");
    case FiModifiedDateDD   :   return krqfiFileInfo.lastModified().date().toString("dd");
    case FiModifiedTimeHH   :   return krqfiFileInfo.lastModified().time().toString("HH");
    case FiModifiedTimeMM   :   return krqfiFileInfo.lastModified().time().toString("mm");
    case FiModifiedTimeSS   :   return krqfiFileInfo.lastModified().time().toString("ss");
    }
    return "";
}


QString IMetaTagLookup::GetValueForTagCodeMusic(const QTableWidgetItem* kpqtwiFileItem, const int kiTagID)
{
    if (kpqtwiFileItem->data(IUIFileList::MusicMeta).isNull())
        return "";

    switch (kiTagID)
    {
    case MuTitle            :   return kpqtwiFileItem->data(IUIFileList::MusicMeta).value<IMetaMusic>().GetTitle();
    case MuArtist           :   return kpqtwiFileItem->data(IUIFileList::MusicMeta).value<IMetaMusic>().GetArtist();
    case MuAlbum            :   return kpqtwiFileItem->data(IUIFileList::MusicMeta).value<IMetaMusic>().GetAlbum();
    case MuTrack            :   return kpqtwiFileItem->data(IUIFileList::MusicMeta).value<IMetaMusic>().GetTrack();
    case MuYear             :   return kpqtwiFileItem->data(IUIFileList::MusicMeta).value<IMetaMusic>().GetYear();
    case MuGenre            :   return kpqtwiFileItem->data(IUIFileList::MusicMeta).value<IMetaMusic>().GetGenre();
    case MuComment          :   return kpqtwiFileItem->data(IUIFileList::MusicMeta).value<IMetaMusic>().GetComment();
    case MuRunTime          :   return kpqtwiFileItem->data(IUIFileList::MusicMeta).value<IMetaMusic>().GetRunTime();
    case MuChannels         :   return kpqtwiFileItem->data(IUIFileList::MusicMeta).value<IMetaMusic>().GetChannels();
    case MuSampleRate       :   return kpqtwiFileItem->data(IUIFileList::MusicMeta).value<IMetaMusic>().GetSampleRate();
    case MuBitRate          :   return kpqtwiFileItem->data(IUIFileList::MusicMeta).value<IMetaMusic>().GetBitRate();
    }
    return "";
}


QString IMetaTagLookup::GetValueForTagCodeExif(const QTableWidgetItem* kpqtwiFileItem, const int kiTagID)
{
    if (kpqtwiFileItem->data(IUIFileList::ExifMeta).isNull())
        return "";

    switch (kiTagID)
    {
    case ExCameraMake       :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetCameraMake();
    case ExCameraModel      :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetCameraModel();
    case ExFNumber          :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetFNumber();
    case ExISOSpeed         :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetISOSpeed();
    case ExExposureTime     :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetExposureTime();
    case ExExposureTimeDec  :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetExposureTimeDec();
    case ExFocalLength      :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetFocalLength();
    case ExProgram          :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetProgram();
    case ExPixelDimX        :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetPixelDimX();
    case ExPixelDimY        :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetPixelDimY();
    case ExDateTime         :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetDateTime();
    case ExDate             :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetDate();
    case ExTime             :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetTime();
    case ExDateYYYY         :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetDateYYYY();
    case ExDateYY           :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetDateYY();
    case ExDateMM           :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetDateMM();
    case ExDateDD           :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetDateDD();
    case ExTimeHH           :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetTimeHH();
    case ExTimeMM           :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetTimeMM();
    case ExTimeSS           :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetTimeSS();
    case ExTimeSubSec       :   return kpqtwiFileItem->data(IUIFileList::ExifMeta).value<IMetaExif>().GetTimeSubSec3Digit();
    }
    return "";
}


QString IMetaTagLookup::ReplaceTagCodesWithValues(const QString & krqstrString, const QList<ITagInfo> & krqlstReplaceNameTags, const QTableWidgetItem* kpqtwiFileItem)
{
    int iSubStringStart = 0;
    QString qstrSubstituted;

    QList<ITagInfo>::const_iterator kitTagInfo;
    for (kitTagInfo = krqlstReplaceNameTags.constBegin() ; kitTagInfo != krqlstReplaceNameTags.constEnd() ; ++kitTagInfo)
    {
        qstrSubstituted += krqstrString.mid(iSubStringStart, kitTagInfo->m_iStartIndex - iSubStringStart) +  GetValueForTagCode(kpqtwiFileItem, kitTagInfo->m_iTagID);
        iSubStringStart = kitTagInfo->m_iEndIndex+1;
    }

    if (iSubStringStart < krqstrString.length())
        qstrSubstituted += krqstrString.mid(iSubStringStart);


    return qstrSubstituted;
}
