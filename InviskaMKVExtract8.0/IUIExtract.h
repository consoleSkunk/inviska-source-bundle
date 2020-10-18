#ifndef IUIExtract_h
#define IUIExtract_h

#include <QWidget>
#include <QProcess>
#include <QDir>
#include <QCollator>
#include <QCoreApplication>
#include "IMKVTrackInfo.h"
#include "IMKVChaptCueshtTagInfo.h"
#include "IMKVAttachmentInfo.h"
#include "IMKVFileInfo.h"
class QTreeWidget;
class QTreeWidgetItem;
class QListWidget;
class QPushButton;
class QCheckBox;
class QLineEdit;
class IUIMainWindow;
class IComSysMKVToolNix;
class IMKVExtractProcess;
class IDlgExtractProgress;


class IUIExtract : public QWidget
{
    Q_OBJECT

private:
    // Pointer to main window
    IUIMainWindow*              m_pmwMainWindow;

    // Tree widget for displaying files and tracks contained in the files
    QTreeWidget*                m_pqtwFileTree;

    // List of check boxes to extract all instances of that track from the added files
    QListWidget*                m_pqlwBatchExtractList;

    // Indicates whether timestamps/cues should be extracted for selected tracks
    QCheckBox*                  m_pqcbExtractTimestamps;
    QCheckBox*                  m_pqcbExtractCues;

    // Line edit for specifying output directory and button to open directory dialog
    QLineEdit*                  m_pqleOutputDir;
    QPushButton*                m_pqpbOutputDirSelect;

    // Buttons to begin extraction and to clear list
    QPushButton*                m_pqpbClearList;
    QPushButton*                m_pqpbBegin;

    // QColator for comparing filenames so they occur correctly ordered in the list for the user's locale
    QCollator                   m_qcolCollator;

    // Acquires, validates and stores the location of MKVToolNix
    IComSysMKVToolNix*          m_pimkvMKVToolNix;

    // Dialog box that displays progress of extraction jobs
    IDlgExtractProgress*        m_pdepExtractionProgress;

    // Indicates whether we should attempt to detect cuesheets and tags or skip their detection
    bool                        m_bDetectCuesheetsTags;

    // Flag to prevent circular reference when unchecking an attachment
    bool                        m_bAttachmentsGroupModified;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // MKVMerge Variables - Used when reading information from a file for adding to the tree //
    ///////////////////////////////////////////////////////////////////////////////////////////

    // Queue of files to be added to file list
    QStringList                 m_qstrlFileQueue;

    // Indicates if a file is currently being processed, and therefore that any added files shoudl be put on the queue
    bool                        m_bFileBeingProcessed;

    // Process for running MKVMerge and MKVExtract to read information from files
    QProcess                    m_qprocMKVToolNix;

    // Output from MKVMerge Identify
    QByteArray                  m_qbaMKVMergeOutput;

    // Tree widget item for file currently being read
    QTreeWidgetItem*            m_pqtwiFileItem;

    // Tree widget item for attachments that is added in the file item
    QTreeWidgetItem*            m_pqtwiAttatchmentsGroup;

    // Indicates which is the current active process, MKVMerge - reading track/chapter/attachment info, MKVExtract - reading cuesheet, or MKVExtract - reading tags
    int                         m_iActiveProcess;
    enum                        ActiveProcess {MKVMerge, MKVExtractCuesheet, MKVExtractTags};

    // Information about the current file being added
    IMKVFileInfo                m_finfCurrentFileInfo;

    // Information regarding the track currently being read
    IMKVTrackInfo               m_tinfTrackInfo;

    // Information regarding the chapters, cuesheet and tags in the current file
    IMKVChaptCueshtTagInfo      m_cctiChaptCueshtTagInfo;

    // Information regarding the attacment currently being read
    IMKVAttachmentInfo          m_ainfAttachmentInfo;

    // Stores the maximum track numbers for each type so the batch extract list can be correctly populated
    IMKVFileInfo                m_finfMaxTrackValues;

    // Path of the file that cuesheets and tags were extracted to in order to determine if they're present
    QString                     m_qstrCuesheetTagTempFile;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // MKVExtract Variables - Used when extracting elements from files                       //
    ///////////////////////////////////////////////////////////////////////////////////////////

    // Pointer to active MKV extract object
    IMKVExtractProcess*         m_pkepMKVExtractor;

public:
    // Constants for element type.  These values are also used for the BatchRoll value, where video 1 would be 201, video 2 would be 202 etc.
    enum                        ElementType {File = 100, Video = 200, Audio = 300, Subtitles = 400, Chapters = 500, Cuesheet = 600, Tags = 700, AttachmentsGroup = 800, Attachment = 900, Invalid = INT_MAX};

    // Constants for roles under which data is stored
    enum                        DataRoles {FilePathRoll = Qt::UserRole, TrackIDRole = Qt::UserRole, CodecIDRole, CodecSupportedRole};

public:
    IUIExtract(IUIMainWindow* pmwMainWindow);

    // Adds all files passed at the command line
    void ProcessCommandLineParameters();

    // Adds the passed item to the bathc extract list
    void AddToBatchExtractList(const QString & krqstrText, const int kiType);

protected:
    void dragEnterEvent(QDragEnterEvent* pqdragEvent);
    void dropEvent(QDropEvent* pqdropEvent);

private slots:
    // Shows open file dialog for adding files to the tree
    void OpenFilesDialog();
    void OpenDirDialog();

private:
    // Gets next file in queue and starts MKVMerge Identify to read track information, or returns if the queue is empty or if a file is being processed
    void ProcessNextFileInQueue();

    // Processes output from MKVMerge and adds information to tree
    void ProcessMKVMergeOutput();
    void ProcessMKVMergeOutputJSON();
    void ProcessMKVMergeOutputVerbose();

    // Starts MKVExtract process to read cuesheets or tags depending on if MKVExtractCuesheet or MKVExtractTags is passed
    void InitReadCuesheetTags(const int kiActiveProcess);

    // Processes cuesheet and tags file after extraction for by MKVExtract
    void ProcessCuesheetTags();

    // Adds track/attachement/chapters information to tree
    void AddTrackToTree();
    void AddAttachmentToTree();
    void AddChaptersCuesheetTagsToTree(const int kiElementType);

    // Used by above functions to add an item to the tree int he correct order
    void AddItemToTree(QTreeWidgetItem* pqtwiItem);

    // Returns the name of the MKVToolNix component being used by the current process
    QString & GetCurrentMKVToolNixComponentName();

private slots:
    // Functions for pocessing output from MKVMerge
    void MKVToolNixOutputText();
    void MKVToolNixErrorText();
    void MKVToolNixFinished(const int kiExitCode, const QProcess::ExitStatus kqpesExitStatus);
    void MKVToolNixError(const QProcess::ProcessError kqpeError);

    // Called when an item in the tree is changed, this is used to check/unckeck the attachements group items and activate/deactive the begin button
    void FileTreeItemToggled(QTreeWidgetItem* pqtwiItem);

    // Iterates through batch extract list and then file tree to find checked items and sets Begin button enable state
    void SetBeginEnableState();

private:
    // Recursive function used by SetBeginEnableState() to iterate through tree and return true if it finds a checked item
    bool CheckedItemFound(QTreeWidgetItem* ptwiParentItem);

private slots:
    // Shows a File dialog to select the directory where files should be outputted
    void SelectOutputDirectory();

    // Clears file tree and batch extract list
    void ClearList();

    // Begins extraction of selected items
    void BeginExtraction();

    // Called when IMKVExtractProcess completes extraction
    void FileComplete(QTreeWidgetItem* ptwiCompleteItem);

private:
    // Begins extraction of the next file in the tree if one exists
    void ExtractNextFileInTree();

    // Confirms the passed directory exists and if not recursively moved up one directory try to create the full path
    bool ConfirmDirectoryExists(QDir & rqdirDir, const QString & krqstrPath);

public:
    // Aports extraction of the current file
    void AbortCurrentExtraction();

    // Accessor functions
    QString              GetOuptputPath();
    bool                 ExtractTimestamps();
    bool                 ExtractCues();
    bool                 DetectCuesheetsTags()  {return m_bDetectCuesheetsTags;}
    QListWidget*         GetBatchExtractList()  {return m_pqlwBatchExtractList;}
    IComSysMKVToolNix*   GetMKVToolNix()        {return m_pimkvMKVToolNix;}
    IDlgExtractProgress* GetProgressDialog()    {return m_pdepExtractionProgress;}

    void SetDetectCuesheetsTags(const bool kbDetectCuesheetsTags) {m_bDetectCuesheetsTags = kbDetectCuesheetsTags;}
};

#endif // IUIExtract_h
