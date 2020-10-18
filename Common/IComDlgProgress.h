#ifndef IComDlgProgress_h
#define IComDlgProgress_h

#include <QDialog>
class QLabel;
class QProgressBar;
class QWinTaskbarProgress;
class IUIMainWindow;


class IComDlgProgress : public QDialog
{
    Q_OBJECT

private:
    // Message describing current progress
    QLabel*                     m_pqlblMessage;

    // Progress bar and taskbar progress bar
    QProgressBar*               m_pqpbProgress;
    QWinTaskbarProgress*        m_pqtbpTaskbarProgress;

    // Abort button
    QPushButton*                m_pqpbAbort;

    // Indicates if Abort has been clicked
    bool                        m_bAborted;

public:
    IComDlgProgress(IUIMainWindow* pmwMainWindow, const QString & krqstrTitle, const QString & krqstrMessage, const int kiMax, const bool kbDisplayPercent, const int kiAppearDelayMS = 4000);
    ~IComDlgProgress();

    // Updates the displayed message
    void UpdateMessage(const QString & krqstrMessage);

    // Updates the progress
    void UpdateProgress(const int kiCurrentVal);

    // Increments progress value and updates
    void IncrementAndUpdateProgress();

    // Returns if abort has been clised
    bool Aborted() {return m_bAborted;}

private slots:
    // Called when button is clicked this takes action depending on the current status
    void AbortClicked();
};

#endif // IComDlgProgress_h
