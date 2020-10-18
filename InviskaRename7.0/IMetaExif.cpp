#include "IMetaExif.h"
#include "IComMetaExif.h"


IMetaExif::IMetaExif()
{

}


IMetaExif::IMetaExif(IComMetaExif* pmexExifMeta)
{
    m_qstrCameraMake        = pmexExifMeta->GetCameraMake();
    m_qstrCameraModel       = pmexExifMeta->GetCameraModel();
    m_qstrFNumber           = pmexExifMeta->GetFNumber();
    m_qstrISOSpeed          = pmexExifMeta->GetISOSpeed();
    m_qstrExposureTime      = pmexExifMeta->GetExposureTime();
    m_qstrExposureTimeDec   = pmexExifMeta->GetExposureTimeDecimal();
    m_qstrFocalLength       = pmexExifMeta->GetFocalLength();
    m_qstrProgram           = pmexExifMeta->GetProgram();
    m_qstrPixelDimX         = pmexExifMeta->GetPixelDimX();
    m_qstrPixelDimY         = pmexExifMeta->GetPixelDimY();
    m_qstrDateTime          = pmexExifMeta->GetDateTime();
    m_qstrTimeSubSec        = pmexExifMeta->GetTimeSubSec();
}


QString IMetaExif::GetTimeSubSec3Digit()
{
    int iLength = m_qstrTimeSubSec.length();
    switch (iLength)
    {
    case 3 : return m_qstrTimeSubSec;
    case 2 : return m_qstrTimeSubSec + "0";
    case 1 : return m_qstrTimeSubSec + "00";
    case 0 : return "000";
    }

    return m_qstrTimeSubSec.left(3);
}
