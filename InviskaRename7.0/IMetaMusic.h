#ifndef IMetaMusic_h
#define IMetaMusic_h

#include <QObject>
class IComMetaMusic;


class IMetaMusic
{
private:
    // Information from taglib is copied to this class becuase files can't be renamed while they're open in taglib
    QString             m_qstrTitle;
    QString             m_qstrArtist;
    QString             m_qstrAlbum;
    QString             m_qstrTrack;
    QString             m_qstrYear;
    QString             m_qstrGenre;
    QString             m_qstrComment;
    QString             m_qstrRunTime;
    QString             m_qstrChannels;
    QString             m_qstrSampleRate;
    QString             m_qstrBitRate;

public:
    IMetaMusic();
    IMetaMusic(IComMetaMusic* pmmuMusicMeta);

    // Accessor functions
    const QString & GetTitle()      {return m_qstrTitle;}
    const QString & GetArtist()     {return m_qstrArtist;}
    const QString & GetAlbum()      {return m_qstrAlbum;}
    const QString & GetTrack()      {return m_qstrTrack;}
    const QString & GetYear()       {return m_qstrYear;}
    const QString & GetGenre()      {return m_qstrGenre;}
    const QString & GetComment()    {return m_qstrComment;}
    const QString & GetRunTime()    {return m_qstrRunTime;}
    const QString & GetChannels()   {return m_qstrChannels;}
    const QString & GetSampleRate() {return m_qstrSampleRate;}
    const QString & GetBitRate()    {return m_qstrBitRate;}
};

Q_DECLARE_METATYPE(IMetaMusic)
Q_DECLARE_METATYPE(IMetaMusic*)

#endif // IMetaMusic_h
