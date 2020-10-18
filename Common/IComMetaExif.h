#ifndef IComMetaExif_h
#define IComMetaExif_h

#include <QString>
#include "libexif/exif-data.h"


class IComMetaExif
{
public:
    // LibExif data structure for storing Exif Data
    ExifData*                   m_pexdExifData;

public:
    IComMetaExif(const QString & krqstrFilePath);
    ~IComMetaExif();

    // Sets passed string to value returned for specified ID and Tag
    void GetValue(QString & rqstrValue, const ExifIfd kexidID, const ExifTag kextaTag);

    // Returns the tag associated with the passed ID
    const char* GetDescription(const ExifIfd kexidID, const ExifTag kextaTag);

private:
    // Remove spaces on the right of the string
    void TrimSpacesFromEnd(char* rgcBuffer);

public:
    // Indicates if Exif data is pressent in file passed at construction
    bool ExifDataPresent()          {return m_pexdExifData != nullptr;}

    // Functions to get values for tags used by Inviska Rename
    QString GetCameraMake();
    QString GetCameraModel();
    QString GetFNumber();
    QString GetISOSpeed();
    QString GetExposureTime();
    QString GetExposureTimeDecimal();
    QString GetFocalLength();
    QString GetProgram();
    QString GetPixelDimX();
    QString GetPixelDimY();
    QString GetDateTime();
    QString GetDate();
    QString GetTime();
    QString GetDateYYYY();
    QString GetDateYY();
    QString GetDateMM();
    QString GetDateDD();
    QString GetTimeHH();
    QString GetTimeMM();
    QString GetTimeSS();
    QString GetTimeSubSec();

    // Returns true if this file extension can contain Exif inforamtion
    static bool FileCanContainExif(const QString & kqstrExtension);
};

#endif // IComMetaExif_h
