#ifndef IUIStripper_h
#define IUIStripper_h

#include <QWidget>
#include <QTextStream>
#include <QDir>
#include <QCollator>
class QListWidget;
class QListWidgetItem;
class QLineEdit;
class QPushButton;
class QCheckBox;
class IUIMainWindow;


class IUIStripper : public QWidget
{
    Q_OBJECT

private:
    // Pointer to main window so menu bar can be updated with saved renames
    IUIMainWindow*              m_pmwMainWindow;

    // List of files that have been added and associated label
    QListWidget*                m_pqlwFileList;

    // List of styles contained within the added files
    QListWidget*                m_pqlwStyleList;

    // Check box to enable stripping of comments from script
    QCheckBox*                  m_qcbStripComments;

    // Line edit for specifying output directory and button to open directory dialog
    QLineEdit*                  m_pqleOutputDir;
    QPushButton*                m_pqpbOutputDirSelect;

    // Buttons to begin stripping and to clear list
    QPushButton*                m_pqpbClearList;
    QPushButton*                m_pqpbBegin;

    // QColator for comparing filenames so they occur correctly ordered in the list for the user's locale
    QCollator                   m_qcolCollator;

    // List of styles to remove for Styles section and venents
    QStringList                 m_qstrlStylesToRemovStyles;
    QStringList                 m_qstrlStylesToRemovEvents;

    // When true outputted files are saved to a new subfolder, otherwise they are saved with (Stripped) appended to the name
    bool                        m_bSaveFilesToSubFolder;

    // Stores the index which the style name starts at in event lines
    int                         m_iStyleStartIndex;

    // Values to indicate which section fot he file we're currently reading
    enum                        FileSection {Header, Styles, Events};

public:
    IUIStripper(IUIMainWindow* pmwMainWindow);

private slots:
    // Called by a 0ms timer so construction can complete first, this adds all files passed at the command line
    void ProcessCommandLineParameters();

protected:
    void dragEnterEvent(QDragEnterEvent* pqdragEvent);
    void dropEvent(QDropEvent* pqdropEvent);

private:
    // Add passed file to file list and add styles
    void AddFile(const QString & krqstrFilePath);

    // Opens passed file and adds styles to Style List
    void ReadStyles(const QString & krqstrFilePath);

    // Reads the passed stream until it finds the styles section.  Returns true if styles section is found or false otherwise.
    bool SeekToStyles(QTextStream & rqtsrmSubFile);

    // Reads style lines from flies and adds them to style list
    void ReadStyles(QTextStream & rqtsrmSubFile);

    // Extracts style name from style line
    QString ExtractStyleName(const QString & krqstrStyleLine);

    // Adds style to style list if it's not already present
    void AddStyleToList(const QString & krqstrStyleName);

private slots:
    // Perform style stripping
    void BeginStripping();

private:
    // Confirms the passed directory exists and if not recursively moved up one directory try to create the full path
    bool ConfirmDirectoryExists(QDir & rqdirDir, const QString & krqstrPath);

    // Generates string list of checked styles from styles list widget
    void GenerateStylesToRemoveList();

    // Strips generated style list from passed file
    void StripFile(const QString & krqstrFilePath);

    // Generates the name for the output file based ont he input file
    QString GenerateOutputFileName(const QString & krqstrFilePath);

    // Functions to check if a style/event should be kept (i.e. if line line doesn't contain one of the styles to remove)
    bool KeepStyle(const QString & krqstrStyleLine);
    bool KeepEvent(const QString & krqstrEventLine);

    // Sets the m_iStyleStartIndex value to the index at which the style name starts in event lines
    void SetStyleStartIndex(const QString & krqstrEventLine);

    // Returns the index of the end of the event style
    int GetStyleEndIndex(const QString & krqstrEventLine);

private slots:
    // Shows a File dialog to select the directory where files should be outputted
    void SelectOutputDirectory();

    // Clear all files and styles from list
    void ClearList();

    // Functions Responsible for setting the begin button enable state depending on the state of other widgets
    void SetBeginButtonEnableState();
    void SetBeginButtonEnableStateStyleItemChange(QListWidgetItem* pqlwiItem);

    // Show open files dialog
    void OpenFilesDialog();

public:
    // Sets the state of the strip comments checkbox
    void SetStripCommentsCheckbox(const bool kbChecked);

    // Sets save to sub directory flag
    void SetSaveFilesToSubFolder(const bool kbSaveFilesToSubFolder) {m_bSaveFilesToSubFolder = kbSaveFilesToSubFolder;}
};

#endif // IUIStripper_h
