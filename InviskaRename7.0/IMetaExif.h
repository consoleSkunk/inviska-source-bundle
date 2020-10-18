#ifndef IMetaExif_h
#define IMetaExif_h

#include <QObject>
class IComMetaExif;


class IMetaExif
{
public:
    // Exif information associated with tags
    QString             m_qstrCameraMake;
    QString             m_qstrCameraModel;
    QString             m_qstrFNumber;
    QString             m_qstrISOSpeed;
    QString             m_qstrExposureTime;
    QString             m_qstrExposureTimeDec;
    QString             m_qstrFocalLength;
    QString             m_qstrProgram;
    QString             m_qstrPixelDimX;
    QString             m_qstrPixelDimY;
    QString             m_qstrDateTime;
    QString             m_qstrTimeSubSec;

public:
    IMetaExif();
    IMetaExif(IComMetaExif* pmexExifMeta);

    // Accessor functions
    const QString & GetCameraMake()     {return m_qstrCameraMake;}
    const QString & GetCameraModel()    {return m_qstrCameraModel;}
    const QString & GetFNumber()        {return m_qstrFNumber;}
    const QString & GetISOSpeed()       {return m_qstrISOSpeed;}
    const QString & GetExposureTime()   {return m_qstrExposureTime;}
    const QString & GetExposureTimeDec(){return m_qstrExposureTimeDec;}
    const QString & GetFocalLength()    {return m_qstrFocalLength;}
    const QString & GetProgram()        {return m_qstrProgram;}
    const QString & GetPixelDimX()      {return m_qstrPixelDimX;}
    const QString & GetPixelDimY()      {return m_qstrPixelDimY;}
    const QString & GetDateTime()       {return m_qstrDateTime;}
    QString GetDate()                   {return m_qstrDateTime.left(10);}
    QString GetTime()                   {return m_qstrDateTime.mid(11, 8);}
    QString GetDateYYYY()               {return m_qstrDateTime.left(4);}
    QString GetDateYY()                 {return m_qstrDateTime.mid(2, 2);}
    QString GetDateMM()                 {return m_qstrDateTime.mid(5, 2);}
    QString GetDateDD()                 {return m_qstrDateTime.mid(8, 2);}
    QString GetTimeHH()                 {return m_qstrDateTime.mid(11, 2);}
    QString GetTimeMM()                 {return m_qstrDateTime.mid(14, 2);}
    QString GetTimeSS()                 {return m_qstrDateTime.mid(17, 2);}
    QString GetTimeSubSec()             {return m_qstrTimeSubSec;}

    // TimeSubSec padded/truncated to three digits
    QString GetTimeSubSec3Digit();
};


Q_DECLARE_METATYPE(IMetaExif)
Q_DECLARE_METATYPE(IMetaExif*)

#endif // IMetaExif_h
