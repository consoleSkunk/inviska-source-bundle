#ifndef IMKVExtractProcess_h
#define IMKVExtractProcess_h

#include <QObject>
#include <QProcess>
class QTreeWidgetItem;
class IUIExtract;
class IDlgExtractProgress;


class IMKVExtractProcess : public QObject
{
    Q_OBJECT

private:
    // Tree widget item that stores the file and track information
    QTreeWidgetItem*            m_pqtwiFileInfo;

    // For gettings things like batch extract list and MKVToolNix location
    IUIExtract*                 m_puiextExtractUI;

    // Dialog box that displays progress of extraction jobs
    IDlgExtractProgress*        m_pdepProgressDlg;

    // Process for running MKVExtract
    QProcess                    m_qprocMKVExtract;

    // Argument list for extraction
    QStringList                 m_qstrlArguments;

    // Path to output extracted files to
    QString                     m_qstrOutputPath;

    // Stores the child indedx of the last tracks item, the chapters item and the attachments item.  All are -1 if no instances present.
    int                         m_iTracksEndIndex;
    int                         m_iChaptersIndex;
    int                         m_iCuesheetIndex;
    int                         m_iTagsIndex;
    int                         m_iAttachmentsIndex;

    // Indicates which phase of the extraction process we're on
    int                         m_iPhase;

public:
    // Values to use to use for m_iPhase
    enum                        Phase {Initial, Tracks, Timestamps, Cues, Chapters, Cuesheet, Tags, Attachmenets, Complete, Aborted, Invalid};

public:
    IMKVExtractProcess(IUIExtract* puiextExtractUI, QTreeWidgetItem* pqtwiFileInfo);

    // Called to begin extraction
    void BeginExtract();

private:
    // Returns the path to output the extracted
    void SetOutputPath();

    // Set the index values for tracks, chapters and attachments
    void SetItemIndecies();

    // Performs the next phase of extraction
    void PerformNextPhase();

    // Functions for extracting elements of different types
    void ExtractTracksTimestampsCues();
    void ExtractChaptersCuesheetTags();
    void ExtractAttachments();

    // Generates file name of extracted elements
    QString GenerateFileName(QTreeWidgetItem* pqtwiTrackInfo, const int kiTrackType, const int kiTrackInstance);

    // Returns true if the track is on the batch extract list
    bool OnBatchExtractList(const int kiTrackType, const int kiTrackInstance);

    // Returnst he phase an item should be extracted on depending on the passed type
    int GetItemPhase(const int kiItemType);

private slots:
    // Functions for pocessing output from MKVExtract
    void MKVExtractOutputText();
    void MKVExtractErrorText();
    void MKVExtractFinished(const int kiExitCode, const QProcess::ExitStatus kqpesExitStatus);
    void MKVExtractError(const QProcess::ProcessError kqpeError);

signals:
    // Segnals when extraction of the current file is complete
    void ExtractionComplete(QTreeWidgetItem* pqtwiCompletedItem);

public:
    // Kills the extraction process and abandons the job
    void AbortExtraction();
};

#endif // IMKVExtractProcess_h
