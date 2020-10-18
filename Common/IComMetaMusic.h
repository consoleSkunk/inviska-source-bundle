#ifndef IComMetaMusic_h
#define IComMetaMusic_h

#include <QString>
#include "fileref.h"
#include "tag.h"


class IComMetaMusic
{
public:
    // File object, also used to access audo properties
    TagLib::FileRef*            m_ptrfrFileRef;

    // Tag information
    TagLib::Tag*                m_ptltagTags;

    // Audio info
    TagLib::AudioProperties*    m_ptlapAudioProp;

public:
    IComMetaMusic(const QString & krqstrFilePath);
    ~IComMetaMusic();

private:
    // Opens files and sets Tag and AudioProperties pointers
    bool OpenFile(const QString & krqstrFilePath);

public:
    // Reopens the file by deleting the current TagLib::FileRef and creating a new one (mostly to get the file into read-write mode)
    bool Reopen(const QString & krqstrFilePath);

    // Saves changes to tages, returning true if sace was successful
    bool SaveTagChanges();

    // Indicates if the file is open
    bool IsOpen();

    // Indicates if the file is open in read only mode (can also indicate that the file could not be opened)
    bool IsReadOnly();

    // Indicates if tag data is pressent in file passed at construction
    bool TagDataPresent()  {return m_ptltagTags != nullptr;}

    // Gets tag and media information
    QString GetTitle();
    QString GetArtist();
    QString GetAlbum();
    QString GetGenre();
    QString GetComment();
    QString GetTrack();
    QString GetTrackTwoDigit();
    int     GetTrackAsInt();
    QString GetYear();
    int     GetYearAsInt();
    QString GetLengthMMSSColon();
    QString GetLengthMMSSSemiColon();
    QString GetLengthInSec();
    int     GetLengthInSecAsInt();
    QString GetChannels();
    int     GetChannelsAsInt();
    QString GetSampleRate();
    QString GetSampleRateHz();
    int     GetSampleRateAsInt();
    QString GetBitRate();
    QString GetBitRatekbps();
    QString GetBitRateSlash();
    int     GetBitRateAsInt();

    // Sets tags to passed value
    void SetTitle(const QString & krqstrTitle);
    void SetArtist(const QString & krqstrArtist);
    void SetAlbum(const QString & krqstrAlbum);
    void SetGenre(const QString & krqstrGenre);
    void SetComment(const QString & krqstrComment);
    void SetTrack(const QString & krqstrTrack);
    void SetTrack(const unsigned int kuiTrack);
    void SetYear(const QString & krqstrYear);
    void SetYear(const unsigned int kuiYear);
};

#endif // IComMetaMusic_h
