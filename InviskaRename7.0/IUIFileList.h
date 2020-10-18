#ifndef IUIFileList_h
#define IUIFileList_h

#include <QSplitter>
#include <QDir>
#include <QFileSystemWatcher>
#include <QFileIconProvider>
#include <QThread>
#include <QStack>
#include "IRenameName.h"
#include "IRenameNumber.h"
#include "IMetaTagLookup.h"
#include "IRenameInvalidCharSub.h"
#include "ISysFileInfoSort.h"
class QTableWidget;
class QTableWidgetItem;
class QMenu;
class IUIMainWindow;
class IUIMenuBar;
class IUIToolBar;
class IUIRenameSettings;


class IUIFileList : public QSplitter
{
    Q_OBJECT

private:
    // Main window
    IUIMainWindow*              m_pmwMainWindow;

    // For reading directory contents and monitoring for changes in directory
    QDir                        m_qdirDirReader;
    QFileSystemWatcher          m_qfswFSWatcher;

    // QDir filter set to either show or hide hidden and system files depending on user preferences
    QDir::Filters               m_qdirfHiddenFileFilter;

    // For sorting file list using natural numbering
    ISysFileInfoSort            m_ifisFileSort;

    // For loading icons
    QFileIconProvider           m_qfipIconProvider;
    QIcon                       m_qicnExeIcon;

    // References to UI elements so we can read settings and enable/disable actions
    IUIMenuBar* &               m_rpuimbMenuBar;
    IUIToolBar* &               m_rpuitbToolBar;
    IUIRenameSettings* &        m_rpuirsRenameSettingsUI;

    // Tables for showing current name and preview of current rename options
    QTableWidget*               m_pqtwNameCurrent;
    QTableWidget*               m_pqtwNamePreview;

    // Stack of directory paths for back and forward buttons
    QStack<QString>             m_qsqstrBackStack;
    QStack<QString>             m_qsqstrForwardStack;

    // Stores the directory and file names for the last rename operation so it can be undone
    QString                     m_qstrUndoDirectory;
    QStringList                 m_qstrUndoRenameFrom;
    QStringList                 m_qstrUndoRenameTo;

    // For looking up values associated with meta tags and replacing those tags in name strings
    IMetaTagLookup              m_mtlMetaTagLookup;

    // Used for substituting invalid characters with alternatives
    IRenameInvalidCharSub       m_rcsInvalidCharSub;

    // Indicates if the directory contents should be automatically refreshed when the contents of the directory changes or a file is modified
    bool                        m_bAutoRefresh;

    // Indicates if files should be opened when double clicked
    bool                        m_bOpenFileWhenDblClicked;

    // Indicates if hidden files are being shown for the directory
    bool                        m_bShowHiddenFiles;

    // Indicates if string comparisons should be case sensitive (for "Replace The Text" feature and file extension comparison)
    bool                        m_bCaseSensitiveCompare;

    // Indicates if a confirmation dialog should be shown before the rename is performed
    bool                        m_bShowConfirmBeforeRename;

    // Indicates if rename settings should be deactivated after the rename operation
    bool                        m_bDeactivateSettingsAfterRename;

    // Indicates if files short be re-sorted after a rename operation
    bool                        m_bReSortFileListAfterRename;

    // Height of each row in table, which can be adjusted to make the rows more or less tightly spaced
    bool                        m_bUseUserDefinedRowHeight;
    int                         m_iUserDefinedRowHeight;

    // Indicates if rows in the preview column should be highlighted when the name is different to the current name
    bool                        m_bNameChangeColourText;
    QColor                      m_qcolNameChangeTextColour;
    bool                        m_bNameChangeHighlightRow;
    QColor                      m_qcolNameChangeHighlightColour;

    // Stores default table font so it can be restored if the user disabled the custom font option
    QFont                       m_qfntDefaultFont;
    bool                        m_bUseAlternativeFont;

    // Used to avoid circular syncing between the two tables
    bool                        m_bSyncSelection;

    // Values for auto numbering
    int                         m_iNumFilesToRename;
    int                         m_iCurrentNumber;
    int                         m_iIncrement;
    int                         m_iNumberCharWidth;

    // Stores the string that represents "My Computer" on Windows (or "This PC" on windows 8.1, or "Computer" in Windows 10)
    QString                     m_qstrMyComputerPath;

    // Indicates if the currnt path being displayed in My Computer
    bool                        m_bDisplayingMyComputer;

    // Indicates which meta tags have been read for the currently open directory
    bool                        m_bMetaTagsReadMusic;
    bool                        m_bMetaTagsReadExif;

    // Variables for right click context menu
    int                         m_iContextMenuRowClicked;
    QMenu*                      m_pqmenuFolderContextMenu;
    QAction*                    m_pqactRenameFolder;
    QAction*                    m_pqactOpenFolder;
    QMenu*                      m_pqmenuFileContextMenu;
    QAction*                    m_pqactRenameFile;
    QAction*                    m_pqactViewProperties;

public:
    // Constants for roles under which data is stored
    enum                        DataRoles {FlaggedForRenme = Qt::UserRole, ToRename, FileInfo, MusicMeta, ExifMeta};

public:
    IUIFileList(IUIMainWindow* pmwMainWindow);

private:
    // Creates and initialieses QTableWidget object
    void ItitialiseTable(QTableWidget* & rpqtwTable, const QString & krqstrTitle);

public:
    // After creation of the window this is called to set initial directory and proccess command line parameters
    void InitAfterCreate();
private:
    void SetInitialDirectory(const QStringList & krqstrlArguments);
    void ProccessOtherCommandLineParam(const QStringList & krqstrlArguments);

public:
    // Set directory to display
    void SetDirectory(QString qstrPath);

    // Should be passed AllEntries, Dirs, Files or Drives, and returns a list which includes/doesn't include hidden files based on current settings
    QFileInfoList GetDirectoryFileList(QDir::Filters m_qdirfFileFilter);

private:
    // Populates tables with directory listing
    void PopulateTablesDirectory();   

    // Populates the table with a list of drives (My Computer)
    void PopulateTablesComputer();

    // Clears contents of both tables and cleares meta read flags
    void ClearTableContents();

    // Removes all files and directories from file system watcher
    void ClearFSWatcher();

    // Updates file info, icons and FS watcher after rename operation
    void RefreshDirectoryPostRename();

    // Hard refresh discards all data in the table (including meta data) and reloads everything
    void RefreshDirectoryHard();

    // Soft refresh compares the table contents to the current directory contents and adds/removes rows as necessary
    void RefreshDirectorySoft();
    void RemoveRows(const QList<int> & krqlstRemoveRows);
    void AddFiles(const QList<int> & krqlstAddIndexes, const QFileInfoList & krqfilFileList);
    void AddFile(const QFileInfo & krqfiNewFile, const int kiRow);

    // Sets flags FlaggedForRenme role based on current rename settings
    void FlagItemsForRenaming();
    void FlagSelectedItemsForRenaming();
    void FlagItemsForRenamingByExtension();

    // These functions check if there are any meta tags in the rename settings, and if so initiate a read of the appropriate tags
    // The functions return false if reading of the meta tags is aborted by the user, or true in all other situations
    bool ReadMetaTags();
    bool CheckListForMetaTags(const QList<ITagInfo> & krqlstTagList);
    bool CheckSettingsForMetaTags();

    // These functions are responsible for reading the meta tags
    bool ReadMetaTagsMusic(const bool kbForceReRead = false);
    void ReadFileMetaTagsMusic(QTableWidgetItem* pqtwiFileItem);
    bool ReadMetaTagsExif(const bool kbForceReRead = false);
    void ReadFileMetaTagsExif(QTableWidgetItem* pqtwiFileItem);

public:
    // Called if invalid character substitutions are changed in the preference menus as substitutions in tags must be redone
    void ReReadMetaTags();

private:
    // Sets hidden file filder based on m_bShowHiddenFiles
    void SetHiddenFileFilter();

private slots:
    // Sets whether to show hidden file state and refreshes if necessary
    void SetHiddenFileState();

    // Sets whether comparisons should be case senstive for "Replace The Text" feature and for file extensions
    void SetCaseSensitiveCompare();

    // When the selected rows in one table chage this functions sync the row selection to the other table and updates the file names
    void SelectionChanged();
    void SyncSelectionXToY(QTableWidget* pqtwSyncFrom, QTableWidget* pqtwSyncTo);

    // When one table scroll position changes these functions sync the other table scroll position to match
    void SyncScrollPreviewToCurrent();
    void SyncScrollCurrentToPreview();

    // Folder Navigation slots
    void OpenDirectoryDialog();
    void OpenFilePropertiesDialog();
    void SetDirectoryFromAddressBar();
    void NavigateBack();
    void NavigateForward();
    void NavigateUp();
    void NavigateRefresh();
    void NavigateToStartDirectory();

    // Navigates to the directory at the passed row, or does nothing if it's a file
    void OpenItemAtRow(const int kiRow);
    void OpenDirectoryContext()             {OpenItemAtRow(m_iContextMenuRowClicked);}

    // Opens a file properties dialog for the file at the specified row
    void OpenFilePropertiesForRow(const int kiRow);
    void OpenFilePropertiesContext()        {OpenFilePropertiesForRow(m_iContextMenuRowClicked);}

    // Opens a dialog for renaming the file at the specified row
    void OpenRenameFileDlgForRow(const int kiRow);
    void OpenRenameFileDlgContext()         {OpenRenameFileDlgForRow(m_iContextMenuRowClicked);}
    void OpenRenameFileDlgForCurrentRow();

    // Displays context menu when file in preview pane is right clicked
    void ShowContextMenu(QPoint qpntClickPoint);

    // Creates right click context menus and associated actions
    void CreateContextMenus();

    // Called when QFileSystemWatcher detects a change in the current directory
    void DirectoryChanged(const QString & krqstrDirectory);
    void FileChanged(const QString & krqstrFile);

    // Prints watch list for debugging purposes
    void PrintWatchList();

    // Moves selected rows up and down for manual re-ordering by user
    void MoveSelectionUp();
    void MoveSelectionDown();

private:
    // Functions for moving individual selection blocks from the selection list up and down
    int MoveSelectionUp(QTableWidget * pqtwTable, const int kiTop, const int kiSelectionTop, const int kiSelectionBottom);
    int MoveSelectionDown(QTableWidget * pqtwTable,  const int kiBottom, const int kiSelectionTop, const int kiSelectionBottom);

public:
    // Updates either the preivew name, extension or both depending on what settings changed
    void GeneratePreviewName();
    void GeneratePreviewExtension();
    void GeneratePreviewNameAndExtension();

    // Refreshes flags indicating which files should be renamed and regenerates previews
    void RenameElementsSettingsChanged();

private:
    // Generates file name or extension for Preview table based on passed settings object
    void GenerateName(QString & rqstrName, const IRenameName & krrsNameRenameSettings, QTableWidgetItem* pqtwiFileItem, const IRenameNumber* const kpnsNumberingSettings = nullptr);

    // Converts passed stringe to title case
    void ConvertNameToTitleCase(QString & rqstrName);

    // Compares the current name to the preivew name and highlights the preivew row if it's different
    void HighlightRowsWithModifiedNames(const bool kbForceRedraw = false);

    // Setup for values for numbering
    void InitNumberingVals(const IRenameNumber & krnsNumberingSettings);
    int NumDigits(int iNum);

    // QDir:rename() doesn't appear to give a reason why the rename failed so these functions try to determine the reason
    void DetermineReasonForFailure(const QString & krqstrCurrentName, const QString & krqstrFailedRename, QString & rqstrFailureReason);

public:
    // Renames files to name shown in preview table
    void PerformRename();

    // Reverts changes made by last rename operation
    void UndoRename();

    // Displays dialog box allowing the user to rename the currently highlighted file
    void RenameHighlightedFile();

    // Accesors for flags
    bool AutoRefreshEnabled()               {return m_bAutoRefresh;}
    void SetAutoRefreshEnabled(const bool kbAutoRefresh);
    bool OpenFileWhenDblClicked()           {return m_bOpenFileWhenDblClicked;}
    void SetOpenFileWhenDblClicked(const bool kbOpenFileWhenDblClicked)                 {m_bOpenFileWhenDblClicked = kbOpenFileWhenDblClicked;}
    bool ShowConfirmBeforeRename()          {return m_bShowConfirmBeforeRename;}
    void SetShowConfirmBeforeRename(const bool kbShowConfirmBeforeRename)               {m_bShowConfirmBeforeRename = kbShowConfirmBeforeRename;}
    bool DeactivateSettingsAfterRename()    {return m_bDeactivateSettingsAfterRename;}
    void SetDeactivateSettingsAfterRename(const bool kbDeactivateSettingsAfterRename)   {m_bDeactivateSettingsAfterRename = kbDeactivateSettingsAfterRename;}
    bool ReSortFileListAfterRename()        {return m_bReSortFileListAfterRename;}
    void SetReSortFilesAfterRename(const bool kbReSortFileListAfterRename)              {m_bReSortFileListAfterRename = kbReSortFileListAfterRename;}

    // Get and set row height
    int GetCurrentRowHeight();
    bool GetUseUserDefinedRowHeight()       {return m_bUseUserDefinedRowHeight;}
    int GetUserDefinedRowHeight()           {return m_iUserDefinedRowHeight;}
    void SetUserDefinedRowHeight(const bool kbUseUserDefinedRowHeight, const int kiRowHeight);
    void SetRowHeightAndIconSize();

    // Functions for getting and setting the highlight colours
    bool GetNameChangeColourText()          {return m_bNameChangeColourText;}
    QColor & GetNameChangeTextColour()      {return m_qcolNameChangeTextColour;}
    bool GetNameChangeHighlightRow()        {return m_bNameChangeHighlightRow;}
    QColor & GetNameChangeHighlightColour() {return m_qcolNameChangeHighlightColour;}
    void UpdateChangeHighlightSettings(const bool kbNameChangeColourText, const QColor & krqcolNameChangeTextColour, const bool kbNameChangeHighlightRow, const QColor & krqcolNameChangeHighlightColour);

    // Functions to get and set font related variables
    bool GetUseAlternativeFont()            {return m_bUseAlternativeFont;}
    void SetUseAlternativeFont(const bool kbUseAlternativeFont) {m_bUseAlternativeFont = kbUseAlternativeFont;}
    void RestoreDefaultFonts();
    const QFont & GetCurrentFont();
    void SetTableFont(const QFont & krqfntNewFont);

    // Other Accessors
    QString & GetMyComputerPath()           {return m_qstrMyComputerPath;}
    IMetaTagLookup & GetMetaTagLookup()     {return m_mtlMetaTagLookup;}
    IRenameInvalidCharSub & GetInvCharSub() {return m_rcsInvalidCharSub;}
    bool GetCaseSensitiveCompare()          {return m_bCaseSensitiveCompare;}
    bool MetaTagsReadMusic()                {return m_bMetaTagsReadMusic;}
    bool MetaTagsReadExif()                 {return m_bMetaTagsReadExif;}

protected:
    // For handling shortcut keys
    void keyPressEvent(QKeyEvent* pqkeEvent);
};

#endif // IUIFileList_h
