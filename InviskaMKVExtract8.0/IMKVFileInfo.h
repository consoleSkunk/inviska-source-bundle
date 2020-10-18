#ifndef IMKVFileInfo_h
#define IMKVFileInfo_h

class IUIExtract;


class IMKVFileInfo
{
    Q_DECLARE_TR_FUNCTIONS(IMKVFileInfo);

private:
    // Number of video, audio and subtitle tracks
    int                         m_iVideoTrackCount;
    int                         m_iAudioTrackCount;
    int                         m_iSubtitleTrackCount;

    // Indicats if the file has chapters, a cuesheet or tags
    bool                        m_bHasChapters;
    bool                        m_bHasCuesheet;
    bool                        m_bHasTags;

    // Number of attachment files
    int                         m_iAttatchmentCount;

public:
    IMKVFileInfo();

    // Resets state of all elements ready to read the next file
    void Reset();

    // Updates the relevent variable based on the track type
    void UpdateTrackCounts(const int kiTrackType);

    // Returns the track count for the passed type
    int GetTrackCountForType(const int kiTrackType);

    // Used by the instance that records the maximum values for all files, this updates the maximums and the batch extract list
    void UpdateFileMaxValues(const IMKVFileInfo & krfinfFileInfo, IUIExtract* puiextExtractUI);
};

#endif // IMKVFileInfo_h
