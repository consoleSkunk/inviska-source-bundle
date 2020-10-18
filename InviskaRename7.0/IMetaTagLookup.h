#ifndef IMetaTagLookup_h
#define IMetaTagLookup_h

#include <QHash>
#include <QString>
#include "IRenameName.h"
class QTableWidgetItem;


class IMetaTagLookup
{
public:
    // For looking up TagID from TagString
    QHash<QString, int>         m_qhashTagIdLookupFiAt;
    QHash<QString, int>         m_qhashTagIdLookupMusic;
    QHash<QString, int>         m_qhashTagIdLookupExif;

    // IDs for tags
    enum TagID
    {InvalidTag,

     FiStart,
     FiCreatedDateTime,  FiCreatedDate,  FiCreatedTime,  FiCreatedDateYYYY,  FiCreatedDateYY, FiCreatedDateMM,  FiCreatedDateDD,  FiCreatedTimeHH,  FiCreatedTimeMM,  FiCreatedTimeSS,
     FiModifiedDateTime, FiModifiedDate, FiModifiedTime, FiModifiedDateYYYY, FiModifiedDateYY, FiModifiedDateMM, FiModifiedDateDD, FiModifiedTimeHH, FiModifiedTimeMM, FiModifiedTimeSS, FiEnd,

     MuStart,
     MuTitle, MuArtist, MuAlbum, MuTrack, MuYear, MuGenre, MuComment, MuRunTime, MuChannels, MuSampleRate, MuBitRate, MuEnd,

     ExStart,
     ExCameraMake, ExCameraModel, ExFNumber, ExISOSpeed, ExExposureTime, ExExposureTimeDec, ExFocalLength, ExProgram, ExPixelDimX, ExPixelDimY,
     ExDateTime, ExDate, ExTime, ExDateYYYY, ExDateYY, ExDateMM, ExDateDD, ExTimeHH, ExTimeMM, ExTimeSS, ExTimeSubSec, ExEnd};

public:
    IMetaTagLookup();

private:
    // Populuates the TagId Lookup Hash
    void PopulateHashFileAttributes();
    void PopulateHashMusic();
    void PopulateHashExif();

    // Returns the value for the specified tag code
    QString GetValueForTagCode(const QTableWidgetItem* kpqtwiFileItem, const int kiTagID);
    QString GetValueForTagCodeFile(const QTableWidgetItem* kpqtwiFileItem, const int kiTagID);
    QString GetValueForTagCodeMusic(const QTableWidgetItem* kpqtwiFileItem, const int kiTagID);
    QString GetValueForTagCodeExif(const QTableWidgetItem* kpqtwiFileItem, const int kiTagID);

public:    
    // Replaces the tag codes in the passed string with the tag value and returns the resulting string
    QString ReplaceTagCodesWithValues(const QString & krqstrString, const QList<ITagInfo> & krqlstReplaceNameTags, const QTableWidgetItem* kpqtwiFileItem);

    // Returns true if the passed tag ID is of the type specified
    bool IsFileTag(const int kiTagID)       {return kiTagID > FiStart && kiTagID < FiEnd;}
    bool IsMusicTag(const int kiTagID)      {return kiTagID > MuStart && kiTagID < MuEnd;}
    bool IsExifTag(const int kiTagID)       {return kiTagID > ExStart && kiTagID < ExEnd;}
};

#endif // IMetaTagLookup_h
