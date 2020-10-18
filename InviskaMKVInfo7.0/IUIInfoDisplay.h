#ifndef IUIInfoDisplay_h
#define IUIInfoDisplay_h

#include <QWidget>
#include <QProcess>
class QLineEdit;
class QTabWidget;
class QPlainTextEdit;
class QTreeWidget;
class QTreeWidgetItem;
class QPushButton;
class IUIMainWindow;
class IDlgScanProgress;
class IComSysMKVToolNix;


class IUIInfoDisplay : public QWidget
{
    Q_OBJECT

private:
    // Pointer to main window
    IUIMainWindow*              m_pmwMainWindow;

    // Displays the current open file
    QLineEdit*                  m_pqleOpenFilePath;

    // Tab widget for selecting between plain text and tree view
    QTabWidget*                 m_pqtwTabWidget;

    // Displays MKVInfo output in plain text
    QPlainTextEdit*             m_pqpteInfoText;

    // Displays MKVMerge identify output in plain text
    QPlainTextEdit*             m_pqpteMergeText;

    // Tree widget which displays the output of MKVInfo in a tree format
    QTreeWidget*                m_pqtwInfoTree;

    // Buttons to open file, save ouptput and clear output
    QPushButton*                m_pqpbOpenFile;
    QPushButton*                m_pqpbSaveInfo;
    QPushButton*                m_pqpbSaveMerge;
    QPushButton*                m_pqpbClear;

    // Acquires, validates and stores the location of MKVToolNix
    IComSysMKVToolNix*          m_pimkvMKVToolNix;

    // Stores path of currently open file
    QString                     m_qstrOpenFilePath;

    // Indicates whether verbose scan is being used
    bool                        m_bScanVerbose;

public:
    // Default tab that is active at startup
    enum                        DefaultTab {MKVInfoTree, MKVInfoText, MKVExtractIdentify};

private:
    //////////////////////////////////////////////////////////////////////////////////////////
    // MKVInfo Variables - Used when reading information from a file for adding to the tree //
    //////////////////////////////////////////////////////////////////////////////////////////

    // Process for running MKVInfo and MKVMerge to read track information from files
    QProcess                    m_qprocMKVToolNix;

    // Shows progress reading file in verbose mode
    IDlgScanProgress*           m_pdspScanProgress;

    // Parent of item currently being added
    QTreeWidgetItem*            m_pqtwiParent;

    // Last item added to the tree, to be used as parent if new item depth is one greater
    QTreeWidgetItem*            m_pqtwiLastItem;

    // Current depth at which we are working, to see if next item is a child or new node
    int                         m_iCurrentDepth;

    // This stores partial lines from the previous block when the output block is split mid line
    QString                     m_qstrPartialLine;

    // Indicates processing phase from defined values
    int                         m_iPhase;
    enum                        Phase {Inactive, Initialising, MKVMerge, MKVInfo, Complete};

public:
    IUIInfoDisplay(IUIMainWindow* pmwMainWindow);

    // Sets the newline character for the OS, taking into account issue in MKVToolNix version 20
    void SetNewLineCharacter();

    // Opens first file passed at the command line
    void ProcessCommandLineParameters();

protected:
    void dragEnterEvent(QDragEnterEvent* pqdragEvent);
    void dropEvent(QDropEvent* pqdropEvent);

private slots:
    // Shows open file dialog for adding files to the tree
    void OpenFileDialog();

private:
    // Used by dropEvent() and OpenFileDialog() top open the accessed file
    bool OpenFile(const QString & krqstrFilePath);

    // Initialise variables and clear contents of window ready for new output
    void InitVariables();

    // Initialises running of MKVMerge identify
    void InitMKVMerge();

    // Initialises running of MKVInfo
    void InitMKVInfo();

private slots:
    // Calls appropriate functions to process output from MKVInfo and MKVMerge
    void MKVToolNixOutputText();

private:
    // Processes output from MKVMerge
    void MergeProcessOutput();

    // Moves cusror to the start after all MKVMerge info added
    void MergeFinaliseProcessing();

    // Processes output from MKVInfo
    void InfoProcessOutput();

    // Sets rqstrLine to the next line in krqstrMKVInfoOutput and updates updates index and depth
    bool InfoGetNextLine(const QString & krqstrMKVInfoOutput, QString & rqstrLine, int & riIndex, int & riDepth);

    // Sets up the UI elements after processing of a file is complete
    void InfoFinaliseProcessing();

    // For verbose mode, this recursive function only expands branches that are present in the standard mode
    void InfoExpandBranchesVerbose(QTreeWidgetItem* pqtwiItem);

    // Purely for verification, this regenerates the lines from the processed data
    // The file produced should be identical to the MKVInfo output (I tested files on Linux and Windows and it is)
    // Only works once so you have to restart the program to test another file, otherwise the last output file will be overwritten
    void InfoVerifyOutput(const QString & krqstLine, const int kiDepth);

private slots:
    // MKVInfo slots called on completion or error
    void MKVToolNixErrorText();
    void MKVToolNixFinished(const int kiExitCode, const QProcess::ExitStatus kqpesExitStatus);
    void MKVToolNixError(const QProcess::ProcessError kqpeError);

    // Aborts current processing job
    void Abort();

    // Clears contents of all tabs
    void Clear();

    // Saves output from MKVInfo and MKVMerge
    void SaveMKVInfoOutput();
    void SaveMKVMergeOutput();
    void SaveOutput(QPlainTextEdit* m_pqpteTextEdit, const QString & krqstrFileSuffix, const QString & krqstrSaveDlgTitle);

    // Set sets scan verbosity level
    void SetEnableVerboseScan(const bool kbScanVerbose);
    void SetScanStandard()              {SetEnableVerboseScan(false);}
    void SetScanVerbose()               {SetEnableVerboseScan(true);}

public:
    // Accessor functions  
    IComSysMKVToolNix* GetMKVToolNix()  {return m_pimkvMKVToolNix;}
    bool               GetScanVerbose() {return m_bScanVerbose;}
};

#endif // IUIInfoDisplay_h
