#include <QFile>
#include "IComMetaExif.h"

// https://libexif.github.io/


// LibExif hasn't been updated since 2012 and is apparently missing some newer Exif tags
// It also doesn't work well with Unicode files on Windows so I might look at switching to an alternative, such as:
// https://www.exiv2.org/
// https://github.com/mayanklahiri/easyexif


IComMetaExif::IComMetaExif(const QString & krqstrFilePath)
{
    m_pexdExifData = nullptr;

    #ifdef Q_OS_WIN
    // libExif opens files with fopen() so it won't work with Unicode file paths on Windows
    // We therefore have to open the file with QFile and create the Exif Data with exif_data_new_from_data()
    QFile qfiImageFile(krqstrFilePath);
    if (!qfiImageFile.open(QIODevice::ReadOnly))
        return;

    const QByteArray qbaImageFileData = qfiImageFile.readAll();
    m_pexdExifData = exif_data_new_from_data(reinterpret_cast<const unsigned char*>(qbaImageFileData.constData()), static_cast<unsigned int>(qbaImageFileData.size()));

    // Using this approach libexif always returns an ExifData object even if there's no Exif data present in the file
    // The camera make seems to always be encoded, so if that's empty we'll assume there's no data
    if (m_pexdExifData != nullptr && GetCameraMake().isEmpty())
    {
        exif_data_free(m_pexdExifData);
        m_pexdExifData = nullptr;
    }
    #else
    m_pexdExifData = exif_data_new_from_file(krqstrFilePath.toLocal8Bit().constData());
    #endif
}


IComMetaExif::~IComMetaExif()
{
    if (m_pexdExifData != nullptr)
        exif_data_unref(m_pexdExifData);
}


void IComMetaExif::GetValue(QString & rqstrValue, const ExifIfd kexidID, const ExifTag kextaTag)
{
    ExifEntry* exenEntry = exif_content_get_entry(m_pexdExifData->ifd[kexidID], kextaTag);
    if (exenEntry != nullptr)
    {
        char rgcBuffer[1024];
        exif_entry_get_value(exenEntry, rgcBuffer, sizeof(rgcBuffer));
        TrimSpacesFromEnd(rgcBuffer);
        if (*rgcBuffer)
            rqstrValue = rgcBuffer;
        else
            rqstrValue.clear();
    }
    else
    {
        rqstrValue.clear();
    }
}


const char* IComMetaExif::GetDescription(const ExifIfd kexidID, const ExifTag kextaTag)
{
    return exif_tag_get_name_in_ifd(kextaTag, kexidID);
}


void IComMetaExif::TrimSpacesFromEnd(char* rgcBuffer)
{
    char *pchEnd = rgcBuffer-1;
    while (*rgcBuffer)
    {
        if (*rgcBuffer != ' ')
            pchEnd = rgcBuffer;
        ++rgcBuffer;
    }
    *++pchEnd = '\0';
}


QString IComMetaExif::GetCameraMake()
{
    QString qstrValue;
    GetValue(qstrValue,     EXIF_IFD_0,     EXIF_TAG_MAKE);

    return qstrValue;
}


QString IComMetaExif::GetCameraModel()
{
    QString qstrValue;
    GetValue(qstrValue,     EXIF_IFD_0,     EXIF_TAG_MODEL);

    return qstrValue;
}


QString IComMetaExif::GetFNumber()
{
    QString qstrValue;
    GetValue(qstrValue,     EXIF_IFD_EXIF,  EXIF_TAG_FNUMBER);

    return qstrValue.mid(2);
}


QString IComMetaExif::GetISOSpeed()
{
    QString qstrValue;
    GetValue(qstrValue,     EXIF_IFD_EXIF,  EXIF_TAG_ISO_SPEED_RATINGS);

    return qstrValue;
}


QString IComMetaExif::GetExposureTime()
{
    QString qstrValue;
    GetValue(qstrValue,     EXIF_IFD_EXIF,  EXIF_TAG_EXPOSURE_TIME);

    if (qstrValue.endsWith(" sec."))
        qstrValue = qstrValue.left(qstrValue.length()-5);

    int iDivIndex = qstrValue.indexOf('/');
    if (iDivIndex != -1)
        qstrValue[iDivIndex] = '_';

    return qstrValue;
}


QString IComMetaExif::GetExposureTimeDecimal()
{
    QString qstrValue;
    GetValue(qstrValue,     EXIF_IFD_EXIF,  EXIF_TAG_EXPOSURE_TIME);

    if (qstrValue.endsWith(" sec."))
        qstrValue = qstrValue.left(qstrValue.length()-5);

    int iDivIndex = qstrValue.indexOf('/');
    if (iDivIndex != -1)
        qstrValue = QString::number(qstrValue.left(iDivIndex).toDouble() / qstrValue.mid(iDivIndex+1).toDouble());

    return qstrValue;
}


QString IComMetaExif::GetFocalLength()
{
    QString qstrValue;
    GetValue(qstrValue,     EXIF_IFD_EXIF,  EXIF_TAG_FOCAL_LENGTH);

    if (qstrValue.endsWith(".0 mm"))
        return qstrValue.left(qstrValue.length()-5);
    if (qstrValue.endsWith(" mm"))
        return qstrValue.left(qstrValue.length()-3);

    return qstrValue;
}


QString IComMetaExif::GetProgram()
{
    QString qstrValue;
    GetValue(qstrValue,     EXIF_IFD_EXIF,  EXIF_TAG_EXPOSURE_PROGRAM);

    return qstrValue;
}


QString IComMetaExif::GetPixelDimX()
{
    QString qstrValue;
    GetValue(qstrValue,     EXIF_IFD_EXIF,     EXIF_TAG_PIXEL_X_DIMENSION);

    return qstrValue;
}


QString IComMetaExif::GetPixelDimY()
{
    QString qstrValue;
    GetValue(qstrValue,     EXIF_IFD_EXIF,     EXIF_TAG_PIXEL_Y_DIMENSION);

    return qstrValue;
}


QString IComMetaExif::GetDateTime()
{
    QString qstrDateTime;
    GetValue(qstrDateTime,  EXIF_IFD_EXIF,  EXIF_TAG_DATE_TIME_ORIGINAL);

    qstrDateTime[4] = '-';
    qstrDateTime[7] = '-';
    qstrDateTime[13] = '.';
    qstrDateTime[16] = '.';

    return qstrDateTime;
}


QString IComMetaExif::GetDate()
{
    QString qstrDateTime;
    GetValue(qstrDateTime,  EXIF_IFD_EXIF,  EXIF_TAG_DATE_TIME_ORIGINAL);

    qstrDateTime[4] = '-';
    qstrDateTime[7] = '-';

    return qstrDateTime.left(10);
}


QString IComMetaExif::GetTime()
{
    QString qstrDateTime;
    GetValue(qstrDateTime,  EXIF_IFD_EXIF,  EXIF_TAG_DATE_TIME_ORIGINAL);

    qstrDateTime[13] = '.';
    qstrDateTime[16] = '.';

    return qstrDateTime.mid(11, 8);
}


QString IComMetaExif::GetDateYYYY()
{
    QString qstrDateTime;
    GetValue(qstrDateTime,  EXIF_IFD_EXIF,  EXIF_TAG_DATE_TIME_ORIGINAL);

    return qstrDateTime.left(4);
}

QString IComMetaExif::GetDateYY()
{
    QString qstrDateTime;
    GetValue(qstrDateTime,  EXIF_IFD_EXIF,  EXIF_TAG_DATE_TIME_ORIGINAL);

    return qstrDateTime.mid(2, 2);
}


QString IComMetaExif::GetDateMM()
{
    QString qstrDateTime;
    GetValue(qstrDateTime,  EXIF_IFD_EXIF,  EXIF_TAG_DATE_TIME_ORIGINAL);

    return qstrDateTime.mid(5, 2);
}


QString IComMetaExif::GetDateDD()
{
    QString qstrDateTime;
    GetValue(qstrDateTime,  EXIF_IFD_EXIF,  EXIF_TAG_DATE_TIME_ORIGINAL);

    return qstrDateTime.mid(8, 2);
}


QString IComMetaExif::GetTimeHH()
{
    QString qstrDateTime;
    GetValue(qstrDateTime,  EXIF_IFD_EXIF,  EXIF_TAG_DATE_TIME_ORIGINAL);

    return qstrDateTime.mid(11, 2);
}


QString IComMetaExif::GetTimeMM()
{
    QString qstrDateTime;
    GetValue(qstrDateTime,  EXIF_IFD_EXIF,  EXIF_TAG_DATE_TIME_ORIGINAL);

    return qstrDateTime.mid(14, 2);
}


QString IComMetaExif::GetTimeSS()
{
    QString qstrDateTime;
    GetValue(qstrDateTime,  EXIF_IFD_EXIF,  EXIF_TAG_DATE_TIME_ORIGINAL);

    return qstrDateTime.mid(17, 2);
}


QString IComMetaExif::GetTimeSubSec()
{
    QString qstrTimeSubSec;
    GetValue(qstrTimeSubSec,  EXIF_IFD_EXIF,  EXIF_TAG_SUB_SEC_TIME_ORIGINAL);

    return qstrTimeSubSec;
}


bool IComMetaExif::FileCanContainExif(const QString & kqstrExtension)
{
    QString qstrExtension = kqstrExtension.toLower();
    if (qstrExtension == "jpg" || qstrExtension == "jpeg")
        return true;
    return false;
}
