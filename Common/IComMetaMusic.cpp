#include "IComMetaMusic.h"

//http://taglib.org/api/classTagLib_1_1File.html
//http://taglib.org/api/classTagLib_1_1Tag.html
//http://taglib.org/api/classTagLib_1_1AudioProperties.html


IComMetaMusic::IComMetaMusic(const QString & krqstrFilePath)
{
    OpenFile(krqstrFilePath);
}


IComMetaMusic::~IComMetaMusic()
{
    if (m_ptrfrFileRef != nullptr)
        delete m_ptrfrFileRef;
}


bool IComMetaMusic::OpenFile(const QString & krqstrFilePath)
{
    m_ptltagTags = nullptr;
    m_ptlapAudioProp = nullptr;

    #ifdef Q_OS_WIN
    m_ptrfrFileRef = new TagLib::FileRef(reinterpret_cast<const wchar_t*>(krqstrFilePath.constData()));
    #else
    m_ptrfrFileRef = new TagLib::FileRef(TagLib::FileName(krqstrFilePath.toUtf8()));
    #endif

    if (m_ptrfrFileRef->isNull() == false)
    {
        m_ptltagTags = m_ptrfrFileRef->tag();
        m_ptlapAudioProp = m_ptrfrFileRef->audioProperties();
        return true;
    }
    return false;
}


bool IComMetaMusic::Reopen(const QString & krqstrFilePath)
{
    if (m_ptrfrFileRef != nullptr)
        delete m_ptrfrFileRef;

    OpenFile(krqstrFilePath);
    return m_ptltagTags != nullptr;
}


bool IComMetaMusic::SaveTagChanges()
{
    return m_ptrfrFileRef->save();
}


bool IComMetaMusic::IsOpen()
{
    return m_ptrfrFileRef->file()->isOpen();
}


bool IComMetaMusic::IsReadOnly()
{
    return m_ptrfrFileRef->file()->readOnly();
}


QString IComMetaMusic::GetTitle()
{
    if (m_ptltagTags == nullptr)
        return "";
    return QString::fromStdWString(m_ptltagTags->title().toWString());
}


QString IComMetaMusic::GetArtist()
{
    if (m_ptltagTags == nullptr)
        return "";
    return QString::fromStdWString(m_ptltagTags->artist().toWString());
}


QString IComMetaMusic::GetAlbum()
{
    if (m_ptltagTags == nullptr)
        return "";
    return QString::fromStdWString(m_ptltagTags->album().toWString());
}


QString IComMetaMusic::GetGenre()
{
    if (m_ptltagTags == nullptr)
        return "";
    return QString::fromStdWString(m_ptltagTags->genre().toWString());
}


QString IComMetaMusic::GetComment()
{
    if (m_ptltagTags == nullptr)
        return "";
    return QString::fromStdWString(m_ptltagTags->comment().toWString());
}


QString IComMetaMusic::GetTrack()
{
    if (m_ptltagTags == nullptr)
        return "";

    const unsigned int kuiTrack = m_ptltagTags->track();
    if (kuiTrack == 0)
        return "";
    return QString::number(kuiTrack);
}


QString IComMetaMusic::GetTrackTwoDigit()
{
    if (m_ptltagTags == nullptr)
        return "";

    const unsigned int kuiTrack = m_ptltagTags->track();
    if (kuiTrack == 0)
        return "";

    QString qstrTrack;
    qstrTrack.sprintf("%02d", kuiTrack);
    return qstrTrack;
}


int IComMetaMusic::GetTrackAsInt()
{
    if (m_ptltagTags == nullptr)
        return 0;
    return static_cast<int>(m_ptltagTags->track());
}


QString IComMetaMusic::GetYear()
{
    if (m_ptltagTags == nullptr)
        return "";

    const unsigned int kuiYear = m_ptltagTags->year();
    if (kuiYear == 0)
        return "";
    return QString::number(kuiYear);
}


int IComMetaMusic::GetYearAsInt()
{
    if (m_ptltagTags == nullptr)
        return 0;
    return static_cast<int>(m_ptltagTags->year());
}


QString IComMetaMusic::GetLengthMMSSColon()
{
    if (m_ptlapAudioProp == nullptr)
        return "";

    QString qstrLenght;
    const int kiLenghtInSec = m_ptlapAudioProp->length();
    qstrLenght.sprintf("%02d:%02d", kiLenghtInSec/60, kiLenghtInSec%60);
    return qstrLenght;
}


QString IComMetaMusic::GetLengthMMSSSemiColon()
{
    if (m_ptlapAudioProp == nullptr)
        return "";

    QString qstrLength;
    const int kiLenghtInSec = m_ptlapAudioProp->length();
    qstrLength.sprintf("%02d;%02d", kiLenghtInSec/60, kiLenghtInSec%60);
    return qstrLength;
}


QString IComMetaMusic::GetLengthInSec()
{
    if (m_ptlapAudioProp == nullptr)
        return "";
    return QString::number(m_ptlapAudioProp->length());
}


int IComMetaMusic::GetLengthInSecAsInt()
{
    if (m_ptlapAudioProp == nullptr)
        return 0;
    return m_ptlapAudioProp->length();
}


QString IComMetaMusic::GetChannels()
{
    if (m_ptlapAudioProp == nullptr)
        return "";
    return QString::number(m_ptlapAudioProp->channels());
}


int IComMetaMusic::GetChannelsAsInt()
{
    if (m_ptlapAudioProp == nullptr)
        return 0;
    return m_ptlapAudioProp->channels();
}


QString IComMetaMusic::GetSampleRate()
{
    if (m_ptlapAudioProp == nullptr)
        return "";

    return QString::number(m_ptlapAudioProp->sampleRate());
}


QString IComMetaMusic::GetSampleRateHz()
{
    if (m_ptlapAudioProp == nullptr)
        return "";

    QString qstrSampleRate;
    qstrSampleRate.sprintf("%d Hz", m_ptlapAudioProp->sampleRate());
    return qstrSampleRate;
}


int IComMetaMusic::GetSampleRateAsInt()
{
    if (m_ptlapAudioProp == nullptr)
        return 0;
    return m_ptlapAudioProp->sampleRate();
}


QString IComMetaMusic::GetBitRate()
{
    if (m_ptlapAudioProp == nullptr)
        return "";

    return QString::number(m_ptlapAudioProp->bitrate());
}


QString IComMetaMusic::GetBitRatekbps()
{
    if (m_ptlapAudioProp == nullptr)
        return "";

    QString qstrSampleRate;
    qstrSampleRate.sprintf("%d kbps", m_ptlapAudioProp->bitrate());
    return qstrSampleRate;
}


QString IComMetaMusic::GetBitRateSlash()
{
    if (m_ptlapAudioProp == nullptr)
        return "";

    QString qstrSampleRate;
    qstrSampleRate.sprintf("%d kb/s", m_ptlapAudioProp->bitrate());
    return qstrSampleRate;
}


int IComMetaMusic::GetBitRateAsInt()
{
    if (m_ptlapAudioProp == nullptr)
        return 0;
    return m_ptlapAudioProp->bitrate();
}


void IComMetaMusic::SetTitle(const QString & krqstrTitle)
{
    if (m_ptltagTags != nullptr)
        m_ptltagTags->setTitle(krqstrTitle.toStdWString());
}


void IComMetaMusic::SetArtist(const QString & krqstrArtist)
{
    if (m_ptltagTags != nullptr)
        m_ptltagTags->setArtist(krqstrArtist.toStdWString());
}


void IComMetaMusic::SetAlbum(const QString & krqstrAlbum)
{
    if (m_ptltagTags != nullptr)
        m_ptltagTags->setAlbum(krqstrAlbum.toStdWString());
}


void IComMetaMusic::SetGenre(const QString & krqstrGenre)
{
    if (m_ptltagTags != nullptr)
        m_ptltagTags->setGenre(krqstrGenre.toStdWString());
}


void IComMetaMusic::SetComment(const QString & krqstrComment)
{
    if (m_ptltagTags != nullptr)
        m_ptltagTags->setComment(krqstrComment.toStdWString());
}


void IComMetaMusic::SetTrack(const QString & krqstrTrack)
{
    if (m_ptltagTags != nullptr)
    {
        if (krqstrTrack.isEmpty())
            m_ptltagTags->setTrack(0);
        else
            m_ptltagTags->setTrack(krqstrTrack.toUInt());
    }
}


void IComMetaMusic::SetTrack(const unsigned int kuiTrack)
{
    if (m_ptltagTags != nullptr)
        m_ptltagTags->setTrack(kuiTrack);
}


void IComMetaMusic::SetYear(const QString & krqstrYear)
{
    if (m_ptltagTags != nullptr)
    {
        if (krqstrYear.isEmpty())
            m_ptltagTags->setYear(0);
        else
            m_ptltagTags->setYear(krqstrYear.toUInt());
    }
}


void IComMetaMusic::SetYear(const unsigned int kuiYear)
{
    if (m_ptltagTags != nullptr)
        m_ptltagTags->setYear(kuiYear);
}
