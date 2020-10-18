#ifndef IMKVTrackInfo_h
#define IMKVTrackInfo_h

#include <QString>
#include <QJsonObject>
class IMKVFileInfo;


class IMKVTrackInfo
{
    Q_DECLARE_TR_FUNCTIONS(IMKVTrackInfo);

private:
    // Track ID for extraction by MKVExtract
    int                         m_iTrackID;

    // Indicates track type
    int                         m_iTrackType;
    QString                     m_qstrTrackType;

    // Name of track, or blank if not specified
    QString                     m_qstrName;

    // Language of track
    QString                     m_qstrLanguage;

    // Indicates video codec, audio codec or subtitle format
    QString                     m_qstrCodec;
    QString                     m_qstrCodecID;

    // Indicates if the track can be extracted by MKVExtract
    bool                        m_bSupported;

public:
    IMKVTrackInfo();

    // Resets state of all elements ready to read the next track
    void Reset();

    // Updates the file information, sets the passed string to the description and returns the track type
    int GenerateTrackDescription(IMKVFileInfo & rfinfoFileInfo, QString & rqstrDescription);

    // Read track information from passed JSON object
    void ReadTrackInfo(QJsonObject qjobjTrackObject);

    // Read track information from passed Identify verbose chapter line
    void ReadTrackInfo(const QString & krqstrTrackLine);

    // Accessor Functions
    bool    IsSupported() {return m_bSupported;}
    int     GetTrackID()  {return m_iTrackID;}
    QString GetCodecID()  {return m_qstrCodecID;}

private:
    // Sets track type values from passed track type string
    void SetTrackType(const QString & krqstrTrackType);
};

#endif // IMKVTrackInfo_h
