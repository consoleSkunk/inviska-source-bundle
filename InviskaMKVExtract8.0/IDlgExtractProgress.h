#ifndef IDlgExtractProgress_h
#define IDlgExtractProgress_h

#include <QDialog>
class QProgressBar;
class QWinTaskbarProgress;
class QLabel;
class IUIMainWindow;


class IDlgExtractProgress : public QDialog
{
    Q_OBJECT

private:
    // Pointer to main window
    IUIMainWindow*              m_pmwMainWindow;

    // Progress bar which indicates how many files have been processed
    QProgressBar*               m_pqpbFilesProcessed;

    // Progress bar which indiactes progress of the current file extraction
    QProgressBar*               m_pqpbFileProgress;

    // Taskbar progress indicator which indicates the overall progress of the job
    QWinTaskbarProgress*        m_pqtbpTaskbarProgress;

    // Labels that display the main status and a status message
    QLabel*                     m_pqlblStatusMessage;
    QLabel*                     m_pqlblDetailedMessage;

    // Button that acts as abart during extraction and close after extraction
    QPushButton*                m_pqpbAbortClose;

    // Counter of number of files that have been completed
    int                         m_iCompletedFiles;

    // Defines current status
    int                         m_iStatus;
    enum                        Status {Extracting, Aborted, Completed};

public:
    IDlgExtractProgress(IUIMainWindow* pmwMainWindow, const int kiNumFiles);

public:
    // Updates the percentage progress for the current file
    void UpdateFileProgress(const int kiPercentage);

    // Incremements the number of completed files and updates progress bar
    void IncrementFilesProcessed();

    // Updates the name of the file currently being processed
    void SetFileBeingProcessed(const QString & krqstrFileName);

    // Changes messages to indicate complete/aborted and changes m_pqpbAbortClose from Abort to Close
    void SetPhase(const int kiExtractionPhase);
    void SetComplete();
    void SetAborted();

private slots:
    // Called when button is clicked this takes action depending on the current status
    void AbortCloseClicked();
};

#endif // IDlgExtractProgress_h
