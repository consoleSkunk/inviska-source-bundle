#ifndef IDlgScanProgress_h
#define IDlgScanProgress_h

#include <QDialog>
class QLabel;
class QProgressBar;
class QWinTaskbarProgress;
class IUIMainWindow;
class IUIInfoDisplay;


class IDlgScanProgress : public QDialog
{
    Q_OBJECT

private:
    // Labels that display the main status and a status message
    QLabel*                     m_pqlblStatusMessage;

    // Progress bar which indiactes progress of the current file scan
    QProgressBar*               m_pqpbScanProgress;

    // Taskbar progress indicator which indicates shows the current file scan progress on the taskbar icon
    QWinTaskbarProgress*        m_pqtbpTaskbarProgress;

    // Abort button to stop scan
    QPushButton*                m_pqpbAbort;

    // Stores the size/1000, or 0 wen not initialised
    int                         m_iFileSize;

    // To avoid excessive UI updates we keep a track of the number of depth < 3 lines and only update every m_kiProgressUpdateInterval lines
    int                         m_iUpdateIntervalCount;
    const int                   m_kiProgressUpdateInterval = 500;

public:
    IDlgScanProgress(IUIMainWindow* pmwMainWindow, IUIInfoDisplay* puiidInfoDisplay, const QString & krqstrFilePath);

    // Extracts file position from line and updates the progress bar
    void UpdateProgress(const QString & krqstrLine, const int kiDepth);

    // Closes dialog, clears taskbar progress and sets passed pointer to nullptr
    void ScanComplete(IDlgScanProgress* & rpdspScanProgress);
};

#endif // IDlgScanProgress_h
