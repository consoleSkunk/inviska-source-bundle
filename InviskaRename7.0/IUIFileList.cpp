#include <QtWidgets>
#include "IUIFileList.h"
#include "IUIMainWindow.h"
#include "IUIMenuBar.h"
#include "IUIToolBar.h"
#include "IUIRenameSettings.h"
#include "IDlgRenameErrorList.h"
#include "IDlgPreferences.h"
#include "IDlgRenameFile.h"
#include "IComDlgFileProperties.h"
#include "IComDlgProgress.h"
#include "IComUtilityFuncs.h"
#include "IComMetaMusic.h"
#include "IComMetaExif.h"
#include "IComSysSorts.h"
#include "IMetaMusic.h"
#include "IMetaExif.h"


IUIFileList::IUIFileList(IUIMainWindow* pmwMainWindow) : QSplitter(Qt::Horizontal, pmwMainWindow),
                                                         m_ifisFileSort(this),
                                                         m_rpuimbMenuBar(pmwMainWindow->GetMenuBar()),
                                                         m_rpuitbToolBar(pmwMainWindow->GetToolBar()),
                                                         m_rpuirsRenameSettingsUI(pmwMainWindow->GetRenameSettingsUI()),
                                                         m_rcsInvalidCharSub(pmwMainWindow->GetSettings())
{
    setChildrenCollapsible(false);
    m_bSyncSelection = true;
    m_qstrMyComputerPath = "Computer";
    m_pmwMainWindow = pmwMainWindow;
    m_bMetaTagsReadMusic = false;
    m_bMetaTagsReadExif = false;

    // Loading embeded executable and folder icons is slow, so use generic icons
    m_qicnExeIcon = m_qfipIconProvider.icon(QFileInfo("NonExistant.exe"));
    m_qfipIconProvider.setOptions(QFileIconProvider::DontUseCustomDirectoryIcons);

    QSettings & rqsetSettings = pmwMainWindow->GetSettings();
    rqsetSettings.beginGroup("FileList");
    m_bAutoRefresh = rqsetSettings.value("AutoRefreshDirectories", true).toBool();
    m_bOpenFileWhenDblClicked = rqsetSettings.value("OpenFileWhenDblClicked", false).toBool();
    m_bShowHiddenFiles = rqsetSettings.value("ShowHiddenFiles", false).toBool();
    m_bCaseSensitiveCompare = rqsetSettings.value("CaseSensitiveCompare", false).toBool();
    m_bNameChangeColourText = rqsetSettings.value("NameChangeColourText", true).toBool();
    m_qcolNameChangeTextColour.setNamedColor(rqsetSettings.value("NameChangeTextColour", "#ff0000").toString());
    m_bNameChangeHighlightRow = rqsetSettings.value("NameChangeHighlightRow", false).toBool();
    m_qcolNameChangeHighlightColour.setNamedColor(rqsetSettings.value("NameChangeHighlightColour", "#ffff7f").toString());
    m_bUseUserDefinedRowHeight = rqsetSettings.value("UseUserDefinedRowHeight", false).toBool();
    m_iUserDefinedRowHeight = rqsetSettings.value("UserDefinedRowHeight", 0).toInt();
    m_bUseAlternativeFont = rqsetSettings.value("UseAlternativeFont", false).toBool();
    QString qstrFileListFont = rqsetSettings.value("FileListFont", "").toString();
    rqsetSettings.endGroup();

    rqsetSettings.beginGroup("Rename");
    m_bShowConfirmBeforeRename = rqsetSettings.value("ShowConfirmBeforeRename", true).toBool();
    m_bDeactivateSettingsAfterRename = rqsetSettings.value("DeactivateSettingsAfterRename", true).toBool();
    m_bReSortFileListAfterRename = rqsetSettings.value("ReSortFileListAfterRename", false).toBool();
    rqsetSettings.endGroup();

    m_qdirDirReader.setPath("");
    m_qdirDirReader.setSorting(QDir::NoSort);
    SetHiddenFileFilter();

    ItitialiseTable(m_pqtwNameCurrent, tr("Current Name"));
    ItitialiseTable(m_pqtwNamePreview, tr("Preview"));

    m_pqtwNameCurrent->setContextMenuPolicy(Qt::CustomContextMenu);
    CreateContextMenus();

    m_qfntDefaultFont = m_pqtwNameCurrent->font();
    if (m_bUseAlternativeFont && qstrFileListFont.isEmpty() == false)
    {
        QFont qfntFileListFont;
        qfntFileListFont.fromString(qstrFileListFont);
        SetTableFont(qfntFileListFont);
    }

    SetRowHeightAndIconSize();

    QScrollBar *pqsbScrollBarCurrent = m_pqtwNameCurrent->verticalScrollBar();
    QScrollBar *pqsbScrollBarPreview = m_pqtwNamePreview->verticalScrollBar();
    connect(pqsbScrollBarCurrent,   SIGNAL(valueChanged(int)),                  this, SLOT(SyncScrollPreviewToCurrent()));
    connect(pqsbScrollBarPreview,   SIGNAL(valueChanged(int)),                  this, SLOT(SyncScrollCurrentToPreview()));

    connect(m_pqtwNameCurrent,      SIGNAL(itemSelectionChanged()),             this, SLOT(SelectionChanged()));
    connect(m_pqtwNamePreview,      SIGNAL(itemSelectionChanged()),             this, SLOT(SelectionChanged()));

    connect(m_pqtwNameCurrent,      SIGNAL(cellDoubleClicked(int, int)),        this, SLOT(OpenItemAtRow(int)));
    connect(m_pqtwNamePreview,      SIGNAL(cellDoubleClicked(int, int)),        this, SLOT(OpenItemAtRow(int)));

    connect(m_pqtwNameCurrent,      SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowContextMenu(QPoint)));

    connect(&m_qfswFSWatcher,       SIGNAL(directoryChanged(const QString &)),  this, SLOT(DirectoryChanged(const QString &)));
    connect(&m_qfswFSWatcher,       SIGNAL(fileChanged(const QString &)),       this, SLOT(FileChanged(const QString &)));
}


void IUIFileList::ItitialiseTable(QTableWidget* & rpqtwTable, const QString & krqstrTitle)
{
    rpqtwTable = new QTableWidget(this);
    rpqtwTable->setColumnCount(1);

    QTableWidgetItem* pqtwiHeader = new QTableWidgetItem;
    pqtwiHeader->setText(krqstrTitle);
    rpqtwTable->setHorizontalHeaderItem(0, pqtwiHeader);
    rpqtwTable->setStyleSheet("QTableWidget::item { padding: 0px; }");
    rpqtwTable->verticalHeader()->setMinimumSectionSize(16);

    rpqtwTable->setEditTriggers(QAbstractItemView::NoEditTriggers);                     // Items not editable
    rpqtwTable->setWordWrap(false);                                                     // Disable word wrapping when contents won't fit in cell.
    rpqtwTable->verticalHeader()->hide();                                               // No row lables
    rpqtwTable->horizontalHeader()->setSectionsClickable(false);                        // Column headers not clickable
    rpqtwTable->horizontalHeader()->setStretchLastSection(true);                        // Expand last column to fill full space (there's only one column)

    addWidget(rpqtwTable);
}


void IUIFileList::InitAfterCreate()
{
    QStringList qstrlArguments = QCoreApplication::arguments();
    SetInitialDirectory(qstrlArguments);
    ProccessOtherCommandLineParam(qstrlArguments);
}


void IUIFileList::SetInitialDirectory(const QStringList & krqstrlArguments)
{
    // Check for directories passed via command line before using default start directory
    int iNumArgs = krqstrlArguments.size();
    if (iNumArgs > 0)
    {
        QFileInfo qfinfFile;
        for (int iArgIndex = 1 ; iArgIndex < iNumArgs ; ++iArgIndex)
        {
            qfinfFile.setFile(krqstrlArguments.at(iArgIndex));
            if (qfinfFile.exists())
            {
                if (qfinfFile.isDir())
                {
                    SetDirectory(qfinfFile.absoluteFilePath());
                    return;
                }
                else if (qfinfFile.isFile())
                {
                    SetDirectory(qfinfFile.absolutePath());
                    return;
                }
            }
        }
    }

    NavigateToStartDirectory();
}


void IUIFileList::ProccessOtherCommandLineParam(const QStringList & krqstrlArguments)
{
    int iNumArgs = krqstrlArguments.size();
    if (iNumArgs > 0)
    {
        for (int iArgIndex = 1 ; iArgIndex < iNumArgs ; ++iArgIndex)
        {
            if (krqstrlArguments.at(iArgIndex) == "--loadren" && iArgIndex+1 < iNumArgs)
                m_pmwMainWindow->GetMenuBar()->ExecuteRename(krqstrlArguments.at(iArgIndex+1));
        }
    }
}


QFileInfoList IUIFileList::GetDirectoryFileList(QDir::Filters m_qdirfFileFilter)
{
    m_qdirDirReader.setFilter(m_qdirfFileFilter | m_qdirfHiddenFileFilter);
    return m_qdirDirReader.entryInfoList();
}


void IUIFileList::SetDirectory(QString qstrPath)
{
    qstrPath = QDir::fromNativeSeparators(qstrPath);

    #ifdef Q_OS_WIN
    if (qstrPath == m_qdirDirReader.absolutePath() && m_bDisplayingMyComputer == false)
        return;

    if (qstrPath == m_qstrMyComputerPath)
    {
        PopulateTablesComputer();
        return;
    }
    #else
    if (qstrPath == m_qdirDirReader.absolutePath())
        return;
    #endif

    if (m_qdirDirReader.exists(qstrPath) == false)
    {
        QMessageBox::information(this, tr("Invalid Path"), tr("The specified directory is invalid:\n\n%1").arg(qstrPath), QMessageBox::Ok);
        return;
    }

    m_qdirDirReader.setPath(qstrPath);
    PopulateTablesDirectory();
    GeneratePreviewNameAndExtension();
}


void IUIFileList::PopulateTablesDirectory()
{
    m_bDisplayingMyComputer = false;

    ClearTableContents();
    QFileInfoList qfilFileList = m_ifisFileSort.GetSortedFileList();
    m_pqtwNameCurrent->setRowCount(qfilFileList.size());
    m_pqtwNamePreview->setRowCount(qfilFileList.size());

    int iRow = 0;
    QIcon qicnFileIcon;
    QTableWidgetItem* pqtwiCurrentItem;
    QTableWidgetItem* pqtwiPreviewItem;
    QFileInfoList::const_iterator kitFile;
    for (kitFile = qfilFileList.constBegin() ; kitFile != qfilFileList.constEnd() ; ++kitFile)
    {
        pqtwiCurrentItem = new QTableWidgetItem;
        pqtwiCurrentItem->setText(kitFile->fileName());
        pqtwiCurrentItem->setData(FileInfo, QVariant::fromValue(*kitFile));
        m_pqtwNameCurrent->setItem(iRow, 0, pqtwiCurrentItem);

        pqtwiPreviewItem = new QTableWidgetItem;
        m_pqtwNamePreview->setItem(iRow, 0, pqtwiPreviewItem);

        #ifdef Q_OS_WIN
        qicnFileIcon = (kitFile->suffix().toLower() == "exe" ? m_qicnExeIcon : m_qfipIconProvider.icon(*kitFile));
        #else
        qicnFileIcon = m_qfipIconProvider.icon(*kitFile);
        #endif
        pqtwiCurrentItem->setIcon(qicnFileIcon);
        pqtwiPreviewItem->setIcon(qicnFileIcon);

        if (kitFile->isFile())
            m_qfswFSWatcher.addPath(kitFile->filePath());

        ++iRow;
    }

    FlagItemsForRenaming();

    const QString kqstrCurrentPath = m_qdirDirReader.path();
    m_qfswFSWatcher.addPath(kqstrCurrentPath);
    m_rpuitbToolBar->SetAddressBarText(QDir::toNativeSeparators(kqstrCurrentPath));
    m_rpuimbMenuBar->EnableBackAction(!m_qsqstrBackStack.isEmpty());
    m_rpuimbMenuBar->EnableForwardAction(!m_qsqstrForwardStack.isEmpty());
    #ifdef Q_OS_WIN
    m_rpuimbMenuBar->EnableUpAction(true);
    #else
    m_rpuimbMenuBar->EnableUpAction(!m_qdirDirReader.isRoot());
    #endif
}


void IUIFileList::PopulateTablesComputer()
{
    m_bDisplayingMyComputer = true;

    ClearTableContents();
    QFileInfoList qfilFileList = m_qdirDirReader.drives();
    m_pqtwNameCurrent->setRowCount(qfilFileList.size());
    m_pqtwNamePreview->setRowCount(qfilFileList.size());

    int iRow = 0;
    QIcon qicnFileIcon;
    QString qstrDriveName;
    QStorageInfo qsiDriveInfo;
    QTableWidgetItem* pqtwiCurrentItem;
    QTableWidgetItem* pqtwiPreviewItem;
    QFileInfoList::const_iterator kitFile;

    for (kitFile = qfilFileList.constBegin() ; kitFile != qfilFileList.constEnd() ; ++kitFile)
    {
        qsiDriveInfo.setPath(kitFile->path());
        if (qsiDriveInfo.isValid() && qsiDriveInfo.isReady())
            qstrDriveName = qsiDriveInfo.displayName() + " (" + kitFile->path().at(0) + ":)";
        else
            qstrDriveName = tr("Removable Disk") + " (" + kitFile->path().at(0) + ":)";

        qicnFileIcon = m_qfipIconProvider.icon(*kitFile);

        pqtwiCurrentItem = new QTableWidgetItem;
        pqtwiCurrentItem->setText(qstrDriveName);
        pqtwiCurrentItem->setIcon(qicnFileIcon);
        pqtwiCurrentItem->setData(FileInfo, QVariant::fromValue(*kitFile));
        m_pqtwNameCurrent->setItem(iRow, 0, pqtwiCurrentItem);

        pqtwiPreviewItem = new QTableWidgetItem;
        pqtwiPreviewItem->setText(qstrDriveName);
        pqtwiPreviewItem->setIcon(qicnFileIcon);
        m_pqtwNamePreview->setItem(iRow, 0, pqtwiPreviewItem);

        ++iRow;
    }

    m_rpuitbToolBar->SetAddressBarText(m_qstrMyComputerPath);
    m_rpuimbMenuBar->EnableUpAction(false);
    m_rpuimbMenuBar->EnableBackAction(!m_qsqstrBackStack.isEmpty());
    m_rpuimbMenuBar->EnableForwardAction(!m_qsqstrForwardStack.isEmpty());

    // Here we should add the directory to the QFileSystemWatcher list, but you can't put a QFileSystemWatcher on Computer or  m_qdirDirReader.drives().
    // This means the Computer view won't update when drives are added/removed.  Not sure how to handle this.
}


void IUIFileList::ClearTableContents()
{
    m_bMetaTagsReadMusic = false;
    m_bMetaTagsReadExif = false;

    ClearFSWatcher();

    m_bSyncSelection = false;
    m_pqtwNameCurrent->clearContents();
    m_pqtwNamePreview->clearContents();
    m_bSyncSelection = true;
}


void IUIFileList::ClearFSWatcher()
{
    #ifdef QT_DEBUG
    if (m_qfswFSWatcher.directories().isEmpty() == false)
    {
        QStringList qstrlList = m_qfswFSWatcher.directories();
        QStringList::const_iterator itItem;
        for (itItem = qstrlList.constBegin() ; itItem != qstrlList.constEnd() ; ++itItem)
        {
            if (m_qfswFSWatcher.removePath(*itItem) == false)
                qDebug() << "Failed to remove directory: " << *itItem;
        }
    }
    if (m_qfswFSWatcher.files().isEmpty() == false)
    {
        QStringList qstrlList = m_qfswFSWatcher.files();
        QStringList::const_iterator itItem;
        for (itItem = qstrlList.constBegin() ; itItem != qstrlList.constEnd() ; ++itItem)
        {
            if (m_qfswFSWatcher.removePath(*itItem) == false)
                 qDebug() << "Failed to remove file: " << *itItem;
        }
    }
    #else
    if (m_qfswFSWatcher.directories().isEmpty() == false)
        m_qfswFSWatcher.removePaths(m_qfswFSWatcher.directories());
    if (m_qfswFSWatcher.files().isEmpty() == false)
        m_qfswFSWatcher.removePaths(m_qfswFSWatcher.files());
    #endif
}


void IUIFileList::RefreshDirectoryPostRename()
{
    m_qdirDirReader.refresh();
    QFileInfoList qfilFileList = GetDirectoryFileList(QDir::AllEntries);
    m_qfswFSWatcher.addPath(m_qdirDirReader.path());

    int iIndex;
    QList<int> qlstRowsToValidate;
    int iSize = m_pqtwNameCurrent->rowCount();
    qlstRowsToValidate.reserve(iSize);
    for (iIndex = 0 ; iIndex < iSize ; ++iIndex)
        qlstRowsToValidate.append(iIndex);

    QString qstrFileName;
    QFileInfo qfiFile;
    QTableWidgetItem* pqtwiRowItem;
    const int kiFileCount = qfilFileList.size();
    for (int iFileListIndex = 0 ; iFileListIndex < kiFileCount ; ++iFileListIndex)
    {
        qfiFile = qfilFileList.at(iFileListIndex);
        qstrFileName = qfiFile.fileName();
        if (qfiFile.isFile())
            m_qfswFSWatcher.addPath(qfiFile.filePath());

        iSize = qlstRowsToValidate.size();
        for (iIndex = 0 ; iIndex < iSize ; ++iIndex)
        {
            pqtwiRowItem = m_pqtwNameCurrent->item(qlstRowsToValidate.at(iIndex), 0);
            if (pqtwiRowItem->text() == qstrFileName)
            {
                if (qstrFileName != pqtwiRowItem->data(FileInfo).value<QFileInfo>().fileName())
                {
                    #ifdef QT_DEBUG
                    qDebug() << "File Renamed From:" << pqtwiRowItem->data(FileInfo).value<QFileInfo>().fileName() <<  "To:" << qstrFileName;
                    #endif

                    pqtwiRowItem->setData(FileInfo, QVariant::fromValue(qfiFile));
                    pqtwiRowItem->setIcon(m_qfipIconProvider.icon(qfiFile));
                    m_pqtwNamePreview->item(qlstRowsToValidate.at(iIndex), 0)->setIcon(m_qfipIconProvider.icon(qfiFile));
                }

                qlstRowsToValidate.removeAt(iIndex);
                break;
            }
        }
    }

    FlagItemsForRenaming();
    GeneratePreviewNameAndExtension();
}


void IUIFileList::RefreshDirectoryHard()
{
    if (m_bDisplayingMyComputer)
    {
        m_bSyncSelection = false;
        PopulateTablesComputer();
        m_bSyncSelection = true;
    }
    else
    {
        m_bSyncSelection = false;
        m_qdirDirReader.refresh();
        PopulateTablesDirectory();
        m_bSyncSelection = true;

        GeneratePreviewNameAndExtension();
    }
}


void IUIFileList::RefreshDirectorySoft()
{
    if (m_bDisplayingMyComputer)
    {
        RefreshDirectoryHard();
        return;
    }

    m_qdirDirReader.refresh();
    QFileInfoList qfilFileList = m_ifisFileSort.GetSortedFileList();

    QList<int> qlstRowsToRemove;
    QList<int> qlstFilesToAdd;

    int iIndex;
    int iSize = m_pqtwNameCurrent->rowCount();
    qlstRowsToRemove.reserve(iSize);
    for (iIndex = 0 ; iIndex < iSize ; ++iIndex)
        qlstRowsToRemove.append(iIndex);

    QString qstrFileName;
    QTableWidgetItem* pqtwiRowItem;
    const int kiFileCount = qfilFileList.size();
    for (int iFileListIndex = 0 ; iFileListIndex < kiFileCount ; ++iFileListIndex)
    {
        qstrFileName = qfilFileList.at(iFileListIndex).fileName();

        iSize = qlstRowsToRemove.size();
        for (iIndex = 0 ; iIndex < iSize ; ++iIndex)
        {
            pqtwiRowItem = m_pqtwNameCurrent->item(qlstRowsToRemove.at(iIndex), 0);
            if (pqtwiRowItem->text() == qstrFileName)
            {
                qlstRowsToRemove.removeAt(iIndex);
                break;
            }
        }
        if (iIndex == iSize)
            qlstFilesToAdd.push_back(iFileListIndex);
    }

    if (qlstRowsToRemove.isEmpty() == false)
        RemoveRows(qlstRowsToRemove);

    if (qlstFilesToAdd.isEmpty() == false)
        AddFiles(qlstFilesToAdd, qfilFileList);

    if (qlstFilesToAdd.isEmpty() == false || qlstRowsToRemove.isEmpty() == false)
    {
        #ifdef QT_DEBUG
        qDebug() << "Regenerating Names After Directory Change";
        #endif

        FlagItemsForRenaming();
        GeneratePreviewNameAndExtension();
    }
}


void IUIFileList::RemoveRows(const QList<int> & krqlstRemoveRows)
{
    #ifdef QT_DEBUG
    qDebug() << "-=Files To Remove=-";
    for (int iIndex = 0 ; iIndex < krqlstRemoveRows.size() ; ++iIndex)
         qDebug() << m_pqtwNameCurrent->item(krqlstRemoveRows.at(iIndex), 0)->text();
    #endif

    QList<int>::const_reverse_iterator ritRowNum;
    for (ritRowNum = krqlstRemoveRows.rbegin() ; ritRowNum != krqlstRemoveRows.rend() ; ++ritRowNum)
    {
        // Deleted files are automatically be removed, but this is still necessary for changing between show/hide hidden fils
        m_qfswFSWatcher.removePath(m_pqtwNameCurrent->item(*ritRowNum, 0)->data(FileInfo).value<QFileInfo>().filePath());
        m_pqtwNameCurrent->removeRow(*ritRowNum);
        m_pqtwNamePreview->removeRow(*ritRowNum);
    }
}


void IUIFileList::AddFiles(const QList<int> & krqlstAddIndexes, const QFileInfoList & krqfilFileList)
{
    #ifdef QT_DEBUG
    qDebug() << "-=Files To Add=-";
    for (int iIndex = 0 ; iIndex < krqlstAddIndexes.size() ; ++iIndex)
        qDebug() << krqfilFileList.at(krqlstAddIndexes.at(iIndex)).fileName();
    #endif

    int iNextToAddIndex = 0;
    const int kiNumFilesToAdd = krqlstAddIndexes.size();
    while (iNextToAddIndex < kiNumFilesToAdd)
    {
       AddFile(krqfilFileList.at(krqlstAddIndexes.at(iNextToAddIndex)), krqlstAddIndexes.at(iNextToAddIndex));
       ++iNextToAddIndex;
    }
}


void IUIFileList::AddFile(const QFileInfo & krqfiNewFile, const int kiRow)
{
    QTableWidgetItem* pqtwiCurrentItem = new QTableWidgetItem;
    pqtwiCurrentItem->setText(krqfiNewFile.fileName());
    pqtwiCurrentItem->setIcon(m_qfipIconProvider.icon(krqfiNewFile));
    m_pqtwNameCurrent->insertRow(kiRow);
    m_pqtwNameCurrent->setItem(kiRow, 0, pqtwiCurrentItem);

    QTableWidgetItem* pqtwiPreviewItem = new QTableWidgetItem;
    pqtwiPreviewItem->setIcon(m_qfipIconProvider.icon(krqfiNewFile));
    m_pqtwNamePreview->insertRow(kiRow);
    m_pqtwNamePreview->setItem(kiRow, 0, pqtwiPreviewItem);

    pqtwiCurrentItem->setData(FileInfo, QVariant::fromValue(krqfiNewFile));
    if (krqfiNewFile.isFile())
        m_qfswFSWatcher.addPath(krqfiNewFile.filePath());

    if (m_bMetaTagsReadMusic)
        ReadFileMetaTagsMusic(pqtwiCurrentItem);
    if (m_bMetaTagsReadExif)
        ReadFileMetaTagsExif(pqtwiCurrentItem);
}


void IUIFileList::FlagItemsForRenaming()
{
    int iRow = 0;
    int iNumRows = m_pqtwNameCurrent->rowCount();
    int iRenameElements = m_rpuirsRenameSettingsUI->RenameElements();
    if (iRenameElements == IUIRenameSettings::RenameFilesOnly)
    {
        for (iRow = 0 ; iRow < iNumRows ; ++iRow)
            m_pqtwNameCurrent->item(iRow, 0)->setData(FlaggedForRenme, m_pqtwNameCurrent->item(iRow, 0)->data(FileInfo).value<QFileInfo>().isFile());
    }
    else if (iRenameElements == IUIRenameSettings::RenameFoldersOnly)
    {
        for (iRow = 0 ; iRow < iNumRows ; ++iRow)
            m_pqtwNameCurrent->item(iRow, 0)->setData(FlaggedForRenme, m_pqtwNameCurrent->item(iRow, 0)->data(FileInfo).value<QFileInfo>().isDir());
    }
    else if (iRenameElements == IUIRenameSettings::RenameFilesAndFolders)
    {
        for (iRow = 0 ; iRow < iNumRows ; ++iRow)
            m_pqtwNameCurrent->item(iRow, 0)->setData(FlaggedForRenme, true);
    }
    else if (iRenameElements == IUIRenameSettings::RenameSelectedItems)
    {
        FlagSelectedItemsForRenaming();
    }
    else //if (iRenameElements == IUIRenameSettings::RenameFilesWithExtension)
    {
        FlagItemsForRenamingByExtension();
    }

    m_iNumFilesToRename = 0;
    for (iRow = 0 ; iRow < iNumRows ; ++iRow)
    {
        if (m_pqtwNameCurrent->item(iRow, 0)->data(FlaggedForRenme).toBool())
            ++m_iNumFilesToRename;
    }
}


void IUIFileList::FlagSelectedItemsForRenaming()
{
    int iRow;
    const int kiNumRows = m_pqtwNameCurrent->rowCount();
    for (iRow = 0 ; iRow < kiNumRows ; ++iRow)
        m_pqtwNameCurrent->item(iRow, 0)->setData(FlaggedForRenme, false);

    int iRowBottom;
    QList<QTableWidgetSelectionRange> qlqtwsrSelections = m_pqtwNameCurrent->selectedRanges();
    QList<QTableWidgetSelectionRange>::const_iterator kitSelection;
    for (kitSelection = qlqtwsrSelections.constBegin() ; kitSelection != qlqtwsrSelections.constEnd() ; ++kitSelection)
    {
        iRowBottom = kitSelection->bottomRow();
        for (iRow = kitSelection->topRow() ; iRow <= iRowBottom ; ++iRow)
            m_pqtwNameCurrent->item(iRow, 0)->setData(FlaggedForRenme, true);
    }
}


void IUIFileList::FlagItemsForRenamingByExtension()
{
    int iRow;
    const int kiNumRows = m_pqtwNameCurrent->rowCount();
    for (iRow = 0 ; iRow < kiNumRows ; ++iRow)
        m_pqtwNameCurrent->item(iRow, 0)->setData(FlaggedForRenme, false);

    QString qstrExtension;
    QStringList krqstrlExtensionList = m_rpuirsRenameSettingsUI->GetRenameExtensions();
    QStringList::const_iterator kitExtension;
    for (iRow = 0 ; iRow < kiNumRows ; ++iRow)
    {
        if (m_pqtwNameCurrent->item(iRow, 0)->data(FileInfo).value<QFileInfo>().isFile())
        {
            qstrExtension = m_pqtwNameCurrent->item(iRow, 0)->text().mid(m_pqtwNameCurrent->item(iRow, 0)->text().lastIndexOf('.')+1);
            if (m_bCaseSensitiveCompare == false)
                qstrExtension = qstrExtension.toLower();

            for (kitExtension = krqstrlExtensionList.constBegin() ; kitExtension != krqstrlExtensionList.constEnd() ; ++kitExtension)
            {
                if (qstrExtension == *kitExtension)
                {
                    m_pqtwNameCurrent->item(iRow, 0)->setData(FlaggedForRenme, true);
                    break;
                }
            }
        }
    }
}


bool IUIFileList::ReadMetaTags()
{
    if (CheckSettingsForMetaTags() == false)
    {
        m_rpuirsRenameSettingsUI->ClearAll();
        RefreshDirectoryHard();
        return false;
    }
    return true;
}


bool IUIFileList::CheckSettingsForMetaTags()
{
    const IRenameName & krrnamNameSettings = m_rpuirsRenameSettingsUI->GetNameSettings();
    if (CheckListForMetaTags(krrnamNameSettings.m_qlstReplaceNameTags) == false)
        return false;
    if (CheckListForMetaTags(krrnamNameSettings.m_qlstReplaceTheTextWithTags) == false)
        return false;
    if (CheckListForMetaTags(krrnamNameSettings.m_qlstInsertTheTextTags) == false)
        return false;
    if (CheckListForMetaTags(krrnamNameSettings.m_qlstInsertAtStartTags) == false)
        return false;
    if (CheckListForMetaTags(krrnamNameSettings.m_qlstInsertAtEndTags) == false)
        return false;

    const IRenameName & krrnamExtenSettings = m_rpuirsRenameSettingsUI->GetExtenSettings();
    if (CheckListForMetaTags(krrnamExtenSettings.m_qlstReplaceNameTags) == false)
        return false;
    if (CheckListForMetaTags(krrnamExtenSettings.m_qlstReplaceTheTextWithTags) == false)
        return false;
    if (CheckListForMetaTags(krrnamExtenSettings.m_qlstInsertTheTextTags) == false)
        return false;
    if (CheckListForMetaTags(krrnamExtenSettings.m_qlstInsertAtStartTags) == false)
        return false;
    if (CheckListForMetaTags(krrnamExtenSettings.m_qlstInsertAtEndTags) == false)
        return false;

    return true;
}


bool IUIFileList::CheckListForMetaTags(const QList<ITagInfo> & krqlstTagList)
{
    if (krqlstTagList.isEmpty() == false)
    {
        QList<ITagInfo>::const_iterator kitTag;
        for (kitTag = krqlstTagList.constBegin() ; kitTag != krqlstTagList.constEnd() ; ++kitTag)
        {
            if (m_bMetaTagsReadMusic == false && m_mtlMetaTagLookup.IsMusicTag(kitTag->m_iTagID))
            {
                if (ReadMetaTagsMusic() == false)
                    return false;
            }
            else if (m_bMetaTagsReadExif == false && m_mtlMetaTagLookup.IsExifTag(kitTag->m_iTagID))
            {
                if (ReadMetaTagsExif() == false)
                    return false;
            }
        }
    }
    return true;
}


bool IUIFileList::ReadMetaTagsMusic(const bool kbForceReRead)
{
    if (m_bMetaTagsReadMusic == true && kbForceReRead == false)
        return true;

    #ifdef QT_DEBUG
    qDebug() << "Reading Music Meta For:" << QDir::toNativeSeparators(m_qdirDirReader.path());
    #endif

    int iRow = 0;
    const int kiNumRows = m_pqtwNameCurrent->rowCount();

    // Skip over folders since these obviously won't contain any music tags
    while (iRow < kiNumRows && m_pqtwNameCurrent->item(iRow, 0)->data(FileInfo).value<QFileInfo>().isDir())
        ++iRow;  
    const int kiStartRow = iRow;

    IComDlgProgress idprgRenameProgress(m_pmwMainWindow, tr("Reading Music Tags"), "Reading file: ", kiNumRows-iRow, false, 1000);

    QTableWidgetItem* pqtwiFileItem;
    const QString kqstrCurrentPath = m_qdirDirReader.path();
    while (iRow < kiNumRows)
    {
        pqtwiFileItem = m_pqtwNameCurrent->item(iRow, 0);
        idprgRenameProgress.UpdateMessage(tr("Reading file: %1").arg(pqtwiFileItem->text()));
        idprgRenameProgress.UpdateProgress(iRow-kiStartRow+1);

        ReadFileMetaTagsMusic(pqtwiFileItem);

        if (idprgRenameProgress.Aborted())
            return false;

        ++iRow;
    }

    m_bMetaTagsReadMusic = true;
    return true;
}


void IUIFileList::ReadFileMetaTagsMusic(QTableWidgetItem* pqtwiFileItem)
{
    IComMetaMusic mmuMusicMeta(QDir::toNativeSeparators(pqtwiFileItem->data(FileInfo).value<QFileInfo>().absoluteFilePath()));
    if (mmuMusicMeta.TagDataPresent())
    {
        m_rcsInvalidCharSub.PerformSubstitution(mmuMusicMeta);
        pqtwiFileItem->setData(MusicMeta, QVariant::fromValue(IMetaMusic(&mmuMusicMeta)));
    }
}


bool IUIFileList::ReadMetaTagsExif(const bool kbForceReRead)
{
    if (m_bMetaTagsReadExif == true && kbForceReRead == false)
        return true;

    #ifdef QT_DEBUG
    qDebug() << "Reading Exif For:" << QDir::toNativeSeparators(m_qdirDirReader.path());
    #endif

    int iRow = 0;
    const int kiNumRows = m_pqtwNameCurrent->rowCount();

    while (iRow < kiNumRows && m_pqtwNameCurrent->item(iRow, 0)->data(FileInfo).value<QFileInfo>().isDir())
        ++iRow;
    const int kiStartRow = iRow;

    IComDlgProgress idprgRenameProgress(m_pmwMainWindow, tr("Reading Exif Data"), "Reading file: ", kiNumRows-iRow, false, 1000);

    QTableWidgetItem* pqtwiFileItem;
    const QString kqstrCurrentPath = m_qdirDirReader.path();
    while (iRow < kiNumRows)
    {
        pqtwiFileItem = m_pqtwNameCurrent->item(iRow, 0);
        idprgRenameProgress.UpdateMessage(tr("Reading file: %1").arg(pqtwiFileItem->text()));
        idprgRenameProgress.UpdateProgress(iRow-kiStartRow+1);

        ReadFileMetaTagsExif(pqtwiFileItem);

        if (idprgRenameProgress.Aborted())
            return false;

        ++iRow;
    }

    m_bMetaTagsReadExif = true;
    return true;
}


void IUIFileList::ReadFileMetaTagsExif(QTableWidgetItem* pqtwiFileItem)
{
    QFileInfo qfiFileInfo = pqtwiFileItem->data(FileInfo).value<QFileInfo>();

    if (IComMetaExif::FileCanContainExif(qfiFileInfo.suffix()) == false)
        return;

    IComMetaExif mexExifMeta(QDir::toNativeSeparators(qfiFileInfo.absoluteFilePath()));
    if (mexExifMeta.ExifDataPresent())
    {
        m_rcsInvalidCharSub.PerformSubstitution(mexExifMeta);
        pqtwiFileItem->setData(ExifMeta, QVariant::fromValue(IMetaExif(&mexExifMeta)));
    }
}


void IUIFileList::ReReadMetaTags()
{
    if (m_bMetaTagsReadMusic)
        ReadMetaTagsMusic(true);
    if (m_bMetaTagsReadExif)
        ReadMetaTagsExif(true);

    if (m_bMetaTagsReadMusic || m_bMetaTagsReadExif)
        GeneratePreviewNameAndExtension();
}


void IUIFileList::SetHiddenFileFilter()
{
    if (m_bShowHiddenFiles)
        m_qdirfHiddenFileFilter = QDir::NoDotAndDotDot | QDir::Hidden | QDir::System;
    else
        m_qdirfHiddenFileFilter = QDir::NoDotAndDotDot;
}


void IUIFileList::SetHiddenFileState()
{
    bool bShowHiddenFiles = m_pmwMainWindow->GetMenuBar()->ShowHiddenFiles();
    if (m_bShowHiddenFiles != bShowHiddenFiles)
    {
        m_bShowHiddenFiles = bShowHiddenFiles;
        m_pmwMainWindow->GetSettings().setValue("FileList/ShowHiddenFiles", m_bShowHiddenFiles);

        SetHiddenFileFilter();
        RefreshDirectorySoft();
    }
}


void IUIFileList::SetCaseSensitiveCompare()
{
    bool bCaseSensitiveCompare = m_pmwMainWindow->GetMenuBar()->CaseSensitiveCompare();
    if (m_bCaseSensitiveCompare != bCaseSensitiveCompare)
    {
        m_bCaseSensitiveCompare = bCaseSensitiveCompare;
        m_pmwMainWindow->GetSettings().setValue("FileList/CaseSensitiveCompare", m_bCaseSensitiveCompare);

        m_rpuirsRenameSettingsUI->ReadRenameExtensions(m_bCaseSensitiveCompare);
        if (m_rpuirsRenameSettingsUI->RenameElements() == IUIRenameSettings::RenameFilesWithExtension)
            FlagItemsForRenaming();

        GeneratePreviewNameAndExtension();
    }
}


void IUIFileList::SelectionChanged()
{
    if (m_bSyncSelection == false)
            return;
    m_bSyncSelection = false;

    if (sender() == m_pqtwNameCurrent)
        SyncSelectionXToY(m_pqtwNameCurrent, m_pqtwNamePreview);
    else
        SyncSelectionXToY(m_pqtwNamePreview, m_pqtwNameCurrent);

    if (m_rpuirsRenameSettingsUI->RenameElements() == IUIRenameSettings::RenameSelectedItems)
    {
        FlagSelectedItemsForRenaming();
        GeneratePreviewNameAndExtension();
    }

    m_bSyncSelection = true;
}


void IUIFileList::SyncSelectionXToY(QTableWidget* pqtwSyncFrom, QTableWidget* pqtwSyncTo)
{
    pqtwSyncTo->clearSelection();
    QList<QTableWidgetSelectionRange> qlqtwslSelections = pqtwSyncFrom->selectedRanges();
    QList<QTableWidgetSelectionRange>::const_iterator kitSelection;
    for (kitSelection = qlqtwslSelections.constBegin() ; kitSelection != qlqtwslSelections.constEnd() ; ++kitSelection)
        pqtwSyncTo->QTableWidget::setRangeSelected(*kitSelection, true);
}


void IUIFileList::SyncScrollPreviewToCurrent()
{
    QScrollBar *pqsbScrollBarCurrent = m_pqtwNameCurrent->verticalScrollBar();
    QScrollBar *pqsbScrollBarPreview = m_pqtwNamePreview->verticalScrollBar();

    pqsbScrollBarPreview->setValue(pqsbScrollBarCurrent->value());
}


void IUIFileList::SyncScrollCurrentToPreview()
{
    QScrollBar *pqsbScrollBarCurrent = m_pqtwNameCurrent->verticalScrollBar();
    QScrollBar *pqsbScrollBarPreview = m_pqtwNamePreview->verticalScrollBar();

    pqsbScrollBarCurrent->setValue(pqsbScrollBarPreview->value());
}


void IUIFileList::OpenDirectoryDialog()
{
    QFileDialog qfdFileDlg(this);
    qfdFileDlg.setFileMode(QFileDialog::Directory);
    qfdFileDlg.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    qfdFileDlg.setWindowTitle(tr("Open Directory"));

    QSettings & rqsetSettings = m_pmwMainWindow->GetSettings();
    rqsetSettings.beginGroup("Application");
    if (rqsetSettings.value("StartDirectoryHome", false).toBool())
    {
        qfdFileDlg.setDirectory(QDir::homePath());
    }
    else if (rqsetSettings.value("StartDirectorySpecified", false).toBool())
    {
        QString qstrStartDirectory = QDir::fromNativeSeparators(rqsetSettings.value("StartDirectory", "").toString());
        if (qstrStartDirectory.isEmpty() == false)
        {
            qfdFileDlg.setDirectory(qstrStartDirectory);
        }
        else
        {
            #ifdef Q_OS_WIN
            qfdFileDlg.setDirectoryUrl(IComUtilityFuncs::GetMyComputerURL());
            #else
            qfdFileDlg.setDirectory(QDir::homePath());
            #endif
        }
        qfdFileDlg.setDirectory(rqsetSettings.value("StartDirectory", QDir::rootPath()).toString());
    }
    else if (rqsetSettings.value("StartDirectoryRoot", false).toBool())
    {
        #ifdef Q_OS_WIN
        qfdFileDlg.setDirectoryUrl(IComUtilityFuncs::GetMyComputerURL());
        #else
        qfdFileDlg.setDirectory(QDir::rootPath());
        #endif
    }
    else
    {
        #ifdef Q_OS_WIN
        qfdFileDlg.setDirectoryUrl(IComUtilityFuncs::GetMyComputerURL());
        #else
        qfdFileDlg.setDirectory(QDir::homePath());
        #endif
    }
    rqsetSettings.endGroup();

    if (qfdFileDlg.exec())
        SetDirectory(qfdFileDlg.selectedFiles().at(0));
}


void IUIFileList::OpenFilePropertiesDialog()
{
    QFileDialog qfdFileDlg(this);
    qfdFileDlg.setFileMode(QFileDialog::ExistingFile);
    qfdFileDlg.setWindowTitle(tr("Select File To View Tags"));

    if (m_bDisplayingMyComputer)
        qfdFileDlg.setDirectoryUrl(IComUtilityFuncs::GetMyComputerURL());
    else
        qfdFileDlg.setDirectory(m_qdirDirReader.path());

    if (qfdFileDlg.exec())
        new IComDlgFileProperties(qfdFileDlg.selectedFiles().at(0));
}


void IUIFileList::SetDirectoryFromAddressBar()
{
    #ifdef QT_DEBUG
    if (m_rpuitbToolBar->GetAdddressBarText() == "test")
    {
        PrintWatchList();
        return;
    }
    #endif

    QString qstrAddressBarText = m_rpuitbToolBar->GetAdddressBarText();
    #ifdef Q_OS_WIN
    if (m_qdirDirReader.exists(qstrAddressBarText) || qstrAddressBarText == m_qstrMyComputerPath)
    #else
    if (m_qdirDirReader.exists(qstrAddressBarText))
    #endif
    {
        m_qsqstrForwardStack.clear();
        m_qsqstrBackStack.push(m_bDisplayingMyComputer ? m_qstrMyComputerPath : m_qdirDirReader.path());
    }
    SetDirectory(qstrAddressBarText);
    m_rpuitbToolBar->AddressBarSelectAll();
}


void IUIFileList::NavigateBack()
{
    if (m_qsqstrBackStack.isEmpty() == false)
    {
        m_qsqstrForwardStack.push(m_bDisplayingMyComputer ? m_qstrMyComputerPath : m_qdirDirReader.path());
        SetDirectory(m_qsqstrBackStack.pop());
    }
}


void IUIFileList::NavigateForward()
{
    if (m_qsqstrForwardStack.isEmpty() == false)
    {
        m_qsqstrBackStack.push(m_bDisplayingMyComputer ? m_qstrMyComputerPath : m_qdirDirReader.path());
        SetDirectory(m_qsqstrForwardStack.pop());
    }
}


void IUIFileList::NavigateUp()
{
    QString qstrNewDir;
    QString qstrCurrentPath = m_qdirDirReader.path();

    if (m_qdirDirReader.isRoot() == false)
    {
        qstrNewDir = qstrCurrentPath.left(qstrCurrentPath.lastIndexOf('/')+1);
    }
    else
    {
        #ifdef Q_OS_WIN
        qstrNewDir = m_qstrMyComputerPath;
        #else
        return;
        #endif
    }

    m_qsqstrForwardStack.clear();
    m_qsqstrBackStack.push(qstrCurrentPath);
    SetDirectory(qstrNewDir);
}


void IUIFileList::NavigateRefresh()
{
    RefreshDirectoryHard();
}


void IUIFileList::NavigateToStartDirectory()
{
    #ifdef Q_OS_WIN
    QString qstrRootPath = GetMyComputerPath();
    #else
    QString qstrRootPath = QDir::rootPath();
    #endif

    QSettings & rqsetSettings = m_pmwMainWindow->GetSettings();
    rqsetSettings.beginGroup("Application");
    if (rqsetSettings.value("StartDirectoryRoot", IDlgPreferences::StartDirectoryRootDefaultVal()).toBool())
        SetDirectory(qstrRootPath);
    else if (rqsetSettings.value("StartDirectoryHome", IDlgPreferences::StartDirectoryHomeDefaultVal()).toBool())
        SetDirectory(QDir::homePath());
    else if (rqsetSettings.value("StartDirectorySpecified", false).toBool())
        SetDirectory(rqsetSettings.value("StartDirectory", qstrRootPath).toString());
    else
        SetDirectory(qstrRootPath);
    rqsetSettings.endGroup();
    }


void IUIFileList::OpenItemAtRow(const int kiRow)
{
    if (m_pqtwNameCurrent->item(kiRow, 0)->data(FileInfo).value<QFileInfo>().isDir())
    {
        m_qsqstrForwardStack.clear();
        m_qsqstrBackStack.push(m_bDisplayingMyComputer ? m_qstrMyComputerPath : m_qdirDirReader.path());
        SetDirectory(m_pqtwNameCurrent->item(kiRow, 0)->data(FileInfo).value<QFileInfo>().filePath());
    }
    else if (m_pqtwNameCurrent->item(kiRow, 0)->data(FileInfo).value<QFileInfo>().isFile())
    {
        if (m_bOpenFileWhenDblClicked)
        {
            // fromUserInput() is necessary for paths containing spaces as it replaces the space character with the %20 encoding required by QUrl
            QDesktopServices::openUrl(QUrl(QUrl::fromUserInput(m_pqtwNameCurrent->item(kiRow, 0)->data(FileInfo).value<QFileInfo>().absoluteFilePath())));
        }
    }
}


void IUIFileList::OpenFilePropertiesForRow(const int kiRow)
{
    if (m_pqtwNameCurrent->item(kiRow, 0)->data(FileInfo).value<QFileInfo>().isFile())
    {
        new IComDlgFileProperties(m_pqtwNameCurrent->item(kiRow, 0)->data(FileInfo).value<QFileInfo>().filePath());
    }
}


void IUIFileList::OpenRenameFileDlgForRow(const int kiRow)
{
    if (m_bDisplayingMyComputer)
    {
        QMessageBox::information(this, tr("Cannot Rename Drives"), tr("Computer drives cannot be renamed."), QMessageBox::Ok);
        return;
    }

    new IDlgRenameFile(this, m_qdirDirReader, m_pqtwNameCurrent->item(kiRow, 0)->text());
}


void IUIFileList::OpenRenameFileDlgForCurrentRow()
{
    OpenRenameFileDlgForRow(m_pqtwNameCurrent->selectedRanges().first().topRow());
}


void IUIFileList::CreateContextMenus()
{
    m_pqactRenameFolder = new QAction(tr("&Rename Folder..."), this);
    QObject::connect(m_pqactRenameFolder, SIGNAL(triggered()), this, SLOT(OpenRenameFileDlgContext()));

    m_pqactOpenFolder = new QAction(tr("&Open Folder"), this);
    QObject::connect(m_pqactOpenFolder, SIGNAL(triggered()), this, SLOT(OpenDirectoryContext()));

    m_pqmenuFolderContextMenu = new QMenu(this);
    m_pqmenuFolderContextMenu->addAction(m_pqactRenameFolder);
    m_pqmenuFolderContextMenu->addAction(m_pqactOpenFolder);


    m_pqactRenameFile = new QAction(tr("&Rename File..."), this);
    QObject::connect(m_pqactRenameFile, SIGNAL(triggered()), this, SLOT(OpenRenameFileDlgContext()));

    m_pqactViewProperties = new QAction(tr("&View File Properties && Tags..."), m_pmwMainWindow);
    m_pqactViewProperties->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_FileDialogContentsView));
    QObject::connect(m_pqactViewProperties, SIGNAL(triggered()), this, SLOT(OpenFilePropertiesContext()));

    m_pqmenuFileContextMenu = new QMenu(this);
    m_pqmenuFileContextMenu->addAction(m_pqactRenameFile);
    m_pqmenuFileContextMenu->addAction(m_pqactViewProperties);
}


void IUIFileList::ShowContextMenu(QPoint qpntClickPoint)
{
    QModelIndex qmiIndex = m_pqtwNameCurrent->indexAt(qpntClickPoint);
    if (qmiIndex.isValid() == false)
        return;
    m_iContextMenuRowClicked = qmiIndex.row();

    if (m_pqtwNameCurrent->item(qmiIndex.row(), 0)->data(FileInfo).value<QFileInfo>().isDir())
    {
        m_pqactRenameFolder->setEnabled(!m_bDisplayingMyComputer);
        m_pqmenuFolderContextMenu->popup(m_pqtwNameCurrent->viewport()->mapToGlobal(qpntClickPoint));
    }
    else
    {
        m_pqmenuFileContextMenu->popup(m_pqtwNameCurrent->viewport()->mapToGlobal(qpntClickPoint));
    }
}


void IUIFileList::DirectoryChanged(const QString & krqstrDirectory)
{
    #ifdef QT_DEBUG
    qDebug() << "Directory Changed:" << krqstrDirectory;
    #endif

    if (m_bAutoRefresh == false || krqstrDirectory != m_qdirDirReader.path())
    {
        #ifdef QT_DEBUG
        qDebug() << "Ignoring Subdirectory Change:" << krqstrDirectory;
        #endif

        return;
    }

    RefreshDirectorySoft();
}


void IUIFileList::FileChanged(const QString & krqstrFile)
{
    #ifdef QT_DEBUG
    qDebug() << "File Changed:" << krqstrFile;
    #endif

    QFileInfo qfiModifiedFile(krqstrFile);
    if (qfiModifiedFile.exists())
    {
        const QString qstrFileName = qfiModifiedFile.fileName();
        const int kiNumRows = m_pqtwNameCurrent->rowCount();
        for (int iRow = 0 ; iRow < kiNumRows ; ++iRow)
        {
            if (m_pqtwNameCurrent->item(iRow, 0)->text() == qstrFileName)
            {
                QTableWidgetItem* pqtwiTableItem = m_pqtwNameCurrent->item(iRow, 0);

                // Sometimes it generates a signal twice for one modification, so we check it has actually been modified
                if (qfiModifiedFile.lastModified() != pqtwiTableItem->data(FileInfo).value<QFileInfo>().lastModified())
                {
                    #ifdef QT_DEBUG
                    qDebug() << "Processing Change:" << krqstrFile;
                    #endif

                    pqtwiTableItem->setData(FileInfo, QVariant::fromValue(qfiModifiedFile));

                    if (m_bMetaTagsReadMusic)
                        ReadFileMetaTagsMusic(pqtwiTableItem);
                    if (m_bMetaTagsReadExif)
                        ReadFileMetaTagsExif(pqtwiTableItem);
                }

                GeneratePreviewNameAndExtension();
                break;
            }
        }
    }
}


void IUIFileList::PrintWatchList()
{
    #ifdef QT_DEBUG
    qDebug() << "-=Watch List=-";
    QStringList qstrlList = m_qfswFSWatcher.directories();
    QStringList::const_iterator itItem;
    for (itItem = qstrlList.begin() ; itItem != qstrlList.end() ; ++itItem)
        qDebug() << "Dir: " << *itItem;

    qstrlList = m_qfswFSWatcher.files();
    for (itItem = qstrlList.begin() ; itItem != qstrlList.end() ; ++itItem)
        qDebug() << "File:" << *itItem;

    qDebug() << endl;
    #endif
}


void IUIFileList::MoveSelectionUp()
{
    m_bSyncSelection = false;
    m_pqtwNameCurrent->setUpdatesEnabled(false);
    m_pqtwNamePreview->setUpdatesEnabled(false);

    IComTableWidgetSelectionSortAsc twslSelectionSortAsc;
    QList<QTableWidgetSelectionRange> qlitwirSelectedRange = m_pqtwNameCurrent->selectedRanges();
    std::sort(qlitwirSelectedRange.begin(), qlitwirSelectedRange.end(), twslSelectionSortAsc);

    int iTop = 0;
    QList<QTableWidgetSelectionRange>::const_iterator kitSelection;
    for (kitSelection = qlitwirSelectedRange.constBegin() ; kitSelection != qlitwirSelectedRange.constEnd() ; ++kitSelection)
    {
        MoveSelectionUp(m_pqtwNameCurrent, iTop, kitSelection->topRow(), kitSelection->bottomRow());
        iTop = MoveSelectionUp(m_pqtwNamePreview, iTop, kitSelection->topRow(), kitSelection->bottomRow());
    }

    m_pqtwNameCurrent->setUpdatesEnabled(true);
    m_pqtwNamePreview->setUpdatesEnabled(true);
    m_bSyncSelection = true;

    if (m_rpuirsRenameSettingsUI->Numberingenabled())
        GeneratePreviewName();
}


void IUIFileList::MoveSelectionDown()
{
    m_bSyncSelection = false;
    m_pqtwNameCurrent->setUpdatesEnabled(false);
    m_pqtwNamePreview->setUpdatesEnabled(false);

    IComTableWidgetSelectionSortDesc twslSelectionSortDesc;
    QList<QTableWidgetSelectionRange> qlitwirSelectedRange = m_pqtwNameCurrent->selectedRanges();
    std::sort(qlitwirSelectedRange.begin(), qlitwirSelectedRange.end(), twslSelectionSortDesc);

    int iBottom = m_pqtwNameCurrent->rowCount() - 1;
    QList<QTableWidgetSelectionRange>::const_iterator kitSelection;
    for (kitSelection = qlitwirSelectedRange.constBegin() ; kitSelection != qlitwirSelectedRange.constEnd() ; ++kitSelection)
    {
        MoveSelectionDown(m_pqtwNameCurrent, iBottom, kitSelection->topRow(), kitSelection->bottomRow());
        iBottom = MoveSelectionDown(m_pqtwNamePreview, iBottom, kitSelection->topRow(), kitSelection->bottomRow());
    }

    m_pqtwNameCurrent->setUpdatesEnabled(true);
    m_pqtwNamePreview->setUpdatesEnabled(true);
    m_bSyncSelection = true;

    if (m_rpuirsRenameSettingsUI->Numberingenabled())
        GeneratePreviewName();
}


int IUIFileList::MoveSelectionUp(QTableWidget * pqtwTable,  const int kiTop, const int kiSelectionTop, const int kiSelectionBottom)
{
    QTableWidgetItem* pqtwiItemAboveSelection;
    QTableWidgetItem* pqtwiItemShiftUp;
    if (kiSelectionTop != kiTop)
    {
        pqtwiItemAboveSelection = pqtwTable->takeItem(kiSelectionTop-1, 0);
        for (int iRow = kiSelectionTop ; iRow <= kiSelectionBottom ; ++iRow)
        {
            pqtwiItemShiftUp = pqtwTable->takeItem(iRow, 0);
            pqtwTable->setItem(iRow-1, 0, pqtwiItemShiftUp);
        }
        pqtwTable->setItem(kiSelectionBottom, 0, pqtwiItemAboveSelection);
        pqtwTable->setRangeSelected(QTableWidgetSelectionRange(kiSelectionTop,   0, kiSelectionBottom,   0), false);
        pqtwTable->setRangeSelected(QTableWidgetSelectionRange(kiSelectionTop-1, 0, kiSelectionBottom-1, 0), true);

        return kiSelectionBottom;
    }
    return kiSelectionBottom + 1;
}


int IUIFileList::MoveSelectionDown(QTableWidget * pqtwTable,  const int kiBottom, const int kiSelectionTop, const int kiSelectionBottom)
{
    QTableWidgetItem* pqtwiItemBelowSelection;
    QTableWidgetItem* pqtwiItemShiftDown;
    if (kiSelectionBottom != kiBottom)
    {
        pqtwiItemBelowSelection = pqtwTable->takeItem(kiSelectionBottom+1, 0);
        for (int iRow = kiSelectionBottom ; iRow >= kiSelectionTop ; --iRow)
        {
            pqtwiItemShiftDown = pqtwTable->takeItem(iRow, 0);
            pqtwTable->setItem(iRow+1, 0, pqtwiItemShiftDown);
        }
        pqtwTable->setItem(kiSelectionTop, 0, pqtwiItemBelowSelection);
        pqtwTable->setRangeSelected(QTableWidgetSelectionRange(kiSelectionTop,   0, kiSelectionBottom,   0), false);
        pqtwTable->setRangeSelected(QTableWidgetSelectionRange(kiSelectionTop+1, 0, kiSelectionBottom+1, 0), true);

        return kiSelectionTop;
    }
    return kiSelectionTop - 1;
}


void IUIFileList::GeneratePreviewName()
{
    if (m_rpuirsRenameSettingsUI->ClearingSettings() || m_bDisplayingMyComputer || ReadMetaTags() == false)
        return;

    QString qstrFileName;
    QString qstrGeneratedName;
    QString qstrPreviewName;
    QString qstrPreviewExtension;
    QTableWidgetItem* pqtwiFileItem;
    int iExtensionIndexCurrent;
    int iExtensionIndexPreview;

    const IRenameName & krrnamNameSettings = m_rpuirsRenameSettingsUI->GetNameSettings();
    const IRenameNumber & krnumNumberingSettings = m_rpuirsRenameSettingsUI->GetNumberingSettings();
    InitNumberingVals(krnumNumberingSettings);

    const int kiNumRows = m_pqtwNameCurrent->rowCount();
    for (int iRow = 0 ; iRow < kiNumRows ; ++iRow)
    {
        pqtwiFileItem = m_pqtwNameCurrent->item(iRow, 0);
        qstrFileName = pqtwiFileItem->text();
        if (pqtwiFileItem->data(FlaggedForRenme).toBool() == false)
        {
           m_pqtwNamePreview->item(iRow, 0)->setText(qstrFileName);
        }
        else if (pqtwiFileItem->data(FileInfo).value<QFileInfo>().isDir())
        {
            GenerateName(qstrFileName, krrnamNameSettings, pqtwiFileItem, &krnumNumberingSettings);
            m_pqtwNamePreview->item(iRow, 0)->setText(qstrFileName);
        }
        else
        {
            // left() returns entire string if n is less than zero, so this works even if there's no extension
            iExtensionIndexCurrent = qstrFileName.lastIndexOf('.');
            qstrGeneratedName = qstrFileName.left(iExtensionIndexCurrent);
            GenerateName(qstrGeneratedName, krrnamNameSettings, pqtwiFileItem, &krnumNumberingSettings);

            // Use extension from Preview table rather than generate it again since no changes have been made to the extension settings
            qstrPreviewName = m_pqtwNamePreview->item(iRow, 0)->text();
            iExtensionIndexPreview = qstrPreviewName.lastIndexOf('.');

            if (iExtensionIndexPreview == -1 || iExtensionIndexCurrent == -1)
            {
                m_pqtwNamePreview->item(iRow, 0)->setText(qstrGeneratedName);
            }
            else
            {
                qstrPreviewExtension = qstrPreviewName.mid(iExtensionIndexPreview);
                m_pqtwNamePreview->item(iRow, 0)->setText(qstrGeneratedName + qstrPreviewExtension);
            }
        }
    }
    HighlightRowsWithModifiedNames();
}


void IUIFileList::GeneratePreviewExtension()
{
    if (m_rpuirsRenameSettingsUI->ClearingSettings() || m_bDisplayingMyComputer || ReadMetaTags() == false)
        return;

    QString qstrFileName;
    QString qstrGeneratedExtension;
    QString qstrPreviewName;
    QString qstrPreviewFileName;
    QTableWidgetItem* pqtwiFileItem;
    int iExtensionIndex;

    const IRenameName & krrnamExtenSettings = m_rpuirsRenameSettingsUI->GetExtenSettings();

    const int kiNumRows = m_pqtwNameCurrent->rowCount();
    for (int iRow = 0 ; iRow < kiNumRows ; ++iRow)
    {
        pqtwiFileItem = m_pqtwNameCurrent->item(iRow, 0);
        qstrFileName = pqtwiFileItem->text();
        if (pqtwiFileItem->data(FlaggedForRenme).toBool() == false)
        {
           m_pqtwNamePreview->item(iRow, 0)->setText(qstrFileName);
        }
        else if (pqtwiFileItem->data(FileInfo).value<QFileInfo>().isFile())
        {
            iExtensionIndex = qstrFileName.lastIndexOf('.');
            if (iExtensionIndex != -1)
                qstrGeneratedExtension = qstrFileName.mid(qstrFileName.lastIndexOf('.')+1);
            else
                qstrGeneratedExtension.clear();
            GenerateName(qstrGeneratedExtension, krrnamExtenSettings, pqtwiFileItem);

            // Use file name from Preview table rather than generate it again since no changes have been made to the file name settings
            qstrPreviewName = m_pqtwNamePreview->item(iRow, 0)->text();
            qstrPreviewFileName = qstrPreviewName.left(qstrPreviewName.lastIndexOf('.'));

            if (qstrGeneratedExtension.isEmpty())
                m_pqtwNamePreview->item(iRow, 0)->setText(qstrPreviewFileName);
            else
                m_pqtwNamePreview->item(iRow, 0)->setText(qstrPreviewFileName + '.' + qstrGeneratedExtension);
        }
    }
    HighlightRowsWithModifiedNames();
}


void IUIFileList::GeneratePreviewNameAndExtension()
{
    if (m_rpuirsRenameSettingsUI->ClearingSettings() || m_bDisplayingMyComputer || ReadMetaTags() == false)
        return;

    QString qstrFileName;
    QString qstrGeneratedName;
    QString qstrGeneratedExtension;
    QTableWidgetItem* pqtwiFileItem;
    int iExtensionIndex;

    const IRenameName & krrnamNameSettings = m_rpuirsRenameSettingsUI->GetNameSettings();
    const IRenameName & krrnamExtenSettings = m_rpuirsRenameSettingsUI->GetExtenSettings();
    const IRenameNumber & krnumNumberingSettings = m_rpuirsRenameSettingsUI->GetNumberingSettings();
    InitNumberingVals(krnumNumberingSettings);

    const int kiNumRows = m_pqtwNameCurrent->rowCount();
    for (int iRow = 0 ; iRow < kiNumRows ; ++iRow)
    {
        pqtwiFileItem = m_pqtwNameCurrent->item(iRow, 0);
        qstrFileName = pqtwiFileItem->text();
        if (pqtwiFileItem->data(FlaggedForRenme).toBool() == false)
        {
           m_pqtwNamePreview->item(iRow, 0)->setText(qstrFileName);
        }
        else if (pqtwiFileItem->data(FileInfo).value<QFileInfo>().isDir())
        {
            GenerateName(qstrFileName, krrnamNameSettings, pqtwiFileItem, &krnumNumberingSettings);
            m_pqtwNamePreview->item(iRow, 0)->setText(qstrFileName);
        }
        else
        {
            // left() returns entire string if n is less than zero, so this works even if there's no extension
            iExtensionIndex = qstrFileName.lastIndexOf('.');
            qstrGeneratedName = qstrFileName.left(iExtensionIndex);
            GenerateName(qstrGeneratedName, krrnamNameSettings, pqtwiFileItem, &krnumNumberingSettings);

            if (iExtensionIndex != -1)
            {
                qstrGeneratedExtension = qstrFileName.mid(iExtensionIndex+1);
                GenerateName(qstrGeneratedExtension, krrnamExtenSettings, pqtwiFileItem);
                m_pqtwNamePreview->item(iRow, 0)->setText(qstrGeneratedName + '.' + qstrGeneratedExtension);
            }
            else
            {
                m_pqtwNamePreview->item(iRow, 0)->setText(qstrGeneratedName);
            }
        }
    }
    HighlightRowsWithModifiedNames();
}


void IUIFileList::RenameElementsSettingsChanged()
{
    FlagItemsForRenaming();
    GeneratePreviewNameAndExtension();
}


void IUIFileList::GenerateName(QString & rqstrName, const IRenameName & krrsRenameSettings, QTableWidgetItem* pqtwiFileItem, const IRenameNumber* const kpnsNumberingSettings)
{
    if (krrsRenameSettings.m_bReplaceName)
    {
        if (krrsRenameSettings.m_qlstReplaceNameTags.isEmpty() == false)
            rqstrName = m_mtlMetaTagLookup.ReplaceTagCodesWithValues(krrsRenameSettings.m_qstrReplaceName, krrsRenameSettings.m_qlstReplaceNameTags, pqtwiFileItem);
        else
            rqstrName = krrsRenameSettings.m_qstrReplaceName;
    }

    if (krrsRenameSettings.m_bReplaceTheText)
    {
        if (krrsRenameSettings.m_qlstReplaceTheTextWithTags.isEmpty() == false)
            rqstrName.replace(krrsRenameSettings.m_qstrReplaceTheText, m_mtlMetaTagLookup.ReplaceTagCodesWithValues(krrsRenameSettings.m_qstrReplaceTheTextWith, krrsRenameSettings.m_qlstReplaceTheTextWithTags, pqtwiFileItem), m_bCaseSensitiveCompare ? Qt::CaseSensitive : Qt::CaseInsensitive);
        else
            rqstrName.replace(krrsRenameSettings.m_qstrReplaceTheText, krrsRenameSettings.m_qstrReplaceTheTextWith, m_bCaseSensitiveCompare ? Qt::CaseSensitive : Qt::CaseInsensitive);
    }

    if (krrsRenameSettings.m_bInsertTheText)
    {
        if (krrsRenameSettings.m_iInsertTheTextAtPos <= rqstrName.length())
        {
            if (krrsRenameSettings.m_qlstInsertTheTextTags.isEmpty() == false)
                rqstrName.insert(krrsRenameSettings.m_iInsertTheTextAtPos, m_mtlMetaTagLookup.ReplaceTagCodesWithValues(krrsRenameSettings.m_qstrInsertTheText, krrsRenameSettings.m_qlstInsertTheTextTags, pqtwiFileItem));
            else
                rqstrName.insert(krrsRenameSettings.m_iInsertTheTextAtPos, krrsRenameSettings.m_qstrInsertTheText);
        }
    }

    if (krrsRenameSettings.m_bInsertAtStart)
    {
        if (krrsRenameSettings.m_qlstInsertAtStartTags.isEmpty() == false)
            rqstrName.prepend(m_mtlMetaTagLookup.ReplaceTagCodesWithValues(krrsRenameSettings.m_qstrInsertAtStart, krrsRenameSettings.m_qlstInsertAtStartTags, pqtwiFileItem));
        else
            rqstrName.prepend(krrsRenameSettings.m_qstrInsertAtStart);
    }

    if (krrsRenameSettings.m_bInsertAtEnd)
    {
        if (krrsRenameSettings.m_qlstInsertAtEndTags.isEmpty() == false)
            rqstrName.append(m_mtlMetaTagLookup.ReplaceTagCodesWithValues(krrsRenameSettings.m_qstrInsertAtEnd, krrsRenameSettings.m_qlstInsertAtEndTags, pqtwiFileItem));
        else
            rqstrName.append(krrsRenameSettings.m_qstrInsertAtEnd);
    }

    if (krrsRenameSettings.m_bCropAtPos)
       rqstrName.remove(krrsRenameSettings.m_iCropAtPos, krrsRenameSettings.m_iCropAtPosNextNChar);

    if (krrsRenameSettings.m_bLeftCropNChar)
       rqstrName.remove(0, krrsRenameSettings.m_iLeftCropNChar);

    if (krrsRenameSettings.m_bRightCropNChar)
       rqstrName.truncate(rqstrName.length() - krrsRenameSettings.m_iRightCropNChar);

    switch (krrsRenameSettings.m_iChangeCase)
    {
        case IRenameName::CaseNoChange  : break;

        case IRenameName::CaseTitle     : ConvertNameToTitleCase(rqstrName);
                                          break;

        case IRenameName::CaseSentance  : rqstrName = rqstrName.toLower();
                                          rqstrName[0] = rqstrName.at(0).toUpper();
                                          break;

        case IRenameName::CaseLower     : rqstrName = rqstrName.toLower();
                                          break;

        case IRenameName::CaseUpper     : rqstrName = rqstrName.toUpper();
                                          break;
    }

    if (kpnsNumberingSettings == nullptr || kpnsNumberingSettings->m_iNumberingType == IRenameNumber::NumberNoNumbering)
        return;

    QString qstrNumber = QString("%1").arg(m_iCurrentNumber, m_iNumberCharWidth, 10, QChar('0'));

    if (kpnsNumberingSettings->m_iNumberingType == IRenameNumber::NumberAfterName)
    {
        rqstrName.insert(rqstrName.length(), qstrNumber);
    }
    else if (kpnsNumberingSettings->m_iNumberingType == IRenameNumber::NumberBeforeName)
    {
        rqstrName.insert(0, qstrNumber);
    }
    else //if (kpnsNumberingSettings->m_iNumberingType == IRenameNumber::NumberAtPos)
    {
        if (kpnsNumberingSettings->m_iNumberingAtPos <= rqstrName.length())
            rqstrName.insert(kpnsNumberingSettings->m_iNumberingAtPos, qstrNumber);
    }

    m_iCurrentNumber += m_iIncrement;
}


void IUIFileList::ConvertNameToTitleCase(QString & rqstrName)
{
    QString qstrWordBreakChars = " -()[]{}.,;";
    rqstrName = rqstrName.toLower();
    rqstrName[0] = rqstrName[0].toUpper();

    int iLength = rqstrName.length();
    for (int iIndex = 1 ; iIndex < iLength ; ++iIndex)
    {
        if (qstrWordBreakChars.contains(rqstrName[iIndex-1]))
            rqstrName[iIndex] = rqstrName[iIndex].toUpper();
    }
}


void IUIFileList::HighlightRowsWithModifiedNames(const bool kbForceRedraw)
{
    bool bNameChangeColourText   = m_bNameChangeColourText || kbForceRedraw;
    bool bNameChangeHighlightRow = m_bNameChangeHighlightRow || kbForceRedraw;

    QColor qcolDefaultForeground    = (QApplication::palette().text().color());
    QColor qcolDefaultBackground    = (QApplication::palette().base().color());
    QColor qcolHighlightForeground = m_bNameChangeColourText ? m_qcolNameChangeTextColour : qcolDefaultForeground;
    QColor qcolHighlightBackground = m_bNameChangeHighlightRow ? m_qcolNameChangeHighlightColour : qcolDefaultBackground;

    bool bRowChanged;
    bool bFilesToRename = false;
    int iNumFiles = m_pqtwNameCurrent->rowCount();
    for (int iRow = 0 ; iRow < iNumFiles ; ++iRow)
    {
        bRowChanged = m_pqtwNameCurrent->item(iRow, 0)->text() != m_pqtwNamePreview->item(iRow, 0)->text();

        if (bNameChangeColourText)
            m_pqtwNamePreview->item(iRow, 0)->setForeground(bRowChanged ? qcolHighlightForeground : qcolDefaultForeground);

        if (bNameChangeHighlightRow)
            m_pqtwNamePreview->item(iRow, 0)->setBackground(bRowChanged ? qcolHighlightBackground : qcolDefaultBackground);

        if (bFilesToRename == false && bRowChanged == true)
            bFilesToRename = true;
    }

    m_rpuirsRenameSettingsUI->m_pqpbRenameButton->setEnabled(bFilesToRename);
}


void IUIFileList::InitNumberingVals(const IRenameNumber & krnumNumberingSettings)
{
    if (krnumNumberingSettings.m_iNumberingType != IRenameNumber::NumberNoNumbering)
    {
        m_iCurrentNumber    = krnumNumberingSettings.m_iNumberingStartNum;
        m_iIncrement        = krnumNumberingSettings.m_iNumberingIncrement;

        if (krnumNumberingSettings.m_bNumberingZeroFillAuto)
        {
            int iMaxNumber = m_iCurrentNumber + (m_iIncrement * (m_iNumFilesToRename-1));
            m_iNumberCharWidth = NumDigits(iMaxNumber);
        }
        else
        {
            m_iNumberCharWidth  = krnumNumberingSettings.m_iNumberingZeroFill + 1;
        }
    }
}


int IUIFileList::NumDigits(int iNum)
{
    iNum = abs(iNum);
    return (iNum < 10 ? 1 :
           (iNum < 100 ? 2 :
           (iNum < 1000 ? 3 :
           (iNum < 10000 ? 4 :
           (iNum < 100000 ? 5 :
           (iNum < 1000000 ? 6 :
           (iNum < 10000000 ? 7 :
           (iNum < 100000000 ? 8 :
           (iNum < 1000000000 ? 9 :
           10)))))))));
}


void IUIFileList::DetermineReasonForFailure(const QString & krqstrCurrentName, const QString & krqstrFailedRename, QString & rqstrFailureReason)
{
    if (m_qdirDirReader.exists(krqstrFailedRename))
    {
        // Could fail becuase the file already exits, most likely due to a numbering issue
        if (m_rpuirsRenameSettingsUI->GetNumberingSettings().m_iNumberingType != IRenameNumber::NumberNoNumbering)
            rqstrFailureReason = tr("File already exits (possible numbering issue).");
        else
            rqstrFailureReason = tr("File already exits");
    }
    else if (m_qdirDirReader.exists(krqstrCurrentName) == false)
    {
        // If the current name doesn't exist the file may have been removed, renamed or deleted
        rqstrFailureReason = tr("File not found (may have been deleted/moved/renamed).");
    }
    else
    {
        // Aside from that it's most likly the user doesn't have permission to modify the file or that the file is open
        rqstrFailureReason = tr("Insufficient privaleges to modify file or file is open in another application.");
    }
    // Can't fail becuase of invalid characters becuase the QRegularExpression validator prevents the user from entering them
}


void IUIFileList::PerformRename()
{
    IDlgRenameErrorList* preldRenameErrorsDialog = nullptr;
    QString qstrCurrentName, qstrPreviewName, qstrFailureReason;
    const QString kqstrCurrentDir = m_qdirDirReader.path();

    m_qstrUndoDirectory = m_qdirDirReader.path();
    m_qstrUndoRenameFrom.clear();
    m_qstrUndoRenameTo.clear();

    // Clearing the whole file system wath list and re-adding all the files in the directory is far faster than removing files individually before rename
    // and re-adding them individually after rename.  This seems a bit silly, particularly if we're only renaming one or two files, but it's a lot faster this way.
    ClearFSWatcher();

    int iRow;
    int iFilesToRename = 0;
    const int kiNumRows = m_pqtwNameCurrent->rowCount();
    for (iRow = 0 ; iRow < kiNumRows ; ++iRow)
    {
        if (m_pqtwNameCurrent->item(iRow, 0)->data(FlaggedForRenme).toBool() && m_pqtwNameCurrent->item(iRow, 0)->text() != m_pqtwNamePreview->item(iRow, 0)->text())
        {
            ++iFilesToRename;
            m_pqtwNameCurrent->item(iRow, 0)->setData(ToRename, true);
        }
        else
        {
            m_pqtwNameCurrent->item(iRow, 0)->setData(ToRename, false);
        }
    }

    IComDlgProgress idprgRenameProgress(m_pmwMainWindow, tr("Renaming Files"), "Renaming: \nTo: ", iFilesToRename, false, 1000);

    for (iRow = 0 ; iRow < kiNumRows ; ++iRow)
    {
        if (m_pqtwNameCurrent->item(iRow, 0)->data(ToRename).toBool())
        {
            qstrCurrentName = m_pqtwNameCurrent->item(iRow, 0)->text();
            qstrPreviewName = m_pqtwNamePreview->item(iRow, 0)->text();
            idprgRenameProgress.UpdateMessage(tr("Renaming: %1\nTo: %2").arg(qstrCurrentName).arg(qstrPreviewName));

            // Remove trailing spaces from end of file name as this causes files to become inaccessible under Windows
            // It also creates an issue on the Mac where "file.txt" is considered a text file, while "file.txt " is treated as an executable
            qstrPreviewName.endsWith(' ');
            {
                int iPos = qstrPreviewName.length() - 1;
                while (qstrPreviewName.at(iPos) == ' ')
                    --iPos;
                qstrPreviewName = qstrPreviewName.left(iPos+1);
            }

            if (m_qdirDirReader.rename(qstrCurrentName, qstrPreviewName) == false)
            {
                if (preldRenameErrorsDialog == nullptr)
                    preldRenameErrorsDialog = new IDlgRenameErrorList(this);

                DetermineReasonForFailure(qstrCurrentName, qstrPreviewName, qstrFailureReason);
                preldRenameErrorsDialog->AddFileToList(qstrCurrentName, qstrPreviewName, qstrFailureReason);
            }
            else
            {
                m_pqtwNameCurrent->item(iRow, 0)->setText(qstrPreviewName);

                m_qstrUndoRenameFrom.push_back(qstrPreviewName);
                m_qstrUndoRenameTo.push_back(qstrCurrentName);
            }

            idprgRenameProgress.IncrementAndUpdateProgress();
            if (idprgRenameProgress.Aborted())
                break;
        }
    }

    if (preldRenameErrorsDialog != nullptr)
        preldRenameErrorsDialog->ResizeColumnsAndShow();

    if (m_qstrUndoRenameFrom.isEmpty() == false)
        m_rpuirsRenameSettingsUI->m_pqpbUndoButton->setEnabled(true);

    if (m_bReSortFileListAfterRename)
        RefreshDirectoryHard();
    else
        RefreshDirectoryPostRename();
}


void IUIFileList::UndoRename()
{
    if (m_qdirDirReader.path() != m_qstrUndoDirectory)
        SetDirectory(m_qstrUndoDirectory);
    const QString kqstrCurrentDir = m_qdirDirReader.path();

    ClearFSWatcher();

    IDlgRenameErrorList* preldRenameErrorsDialog = nullptr;
    QString qstrFailureReason;

    IComDlgProgress idprgRenameProgress(m_pmwMainWindow, tr("Renaming Files"), "Renaming: file\nTo: file", m_qstrUndoRenameFrom.size(), false, 1000);

    QStringList::const_iterator kitRenameFrom = m_qstrUndoRenameFrom.constBegin();
    QStringList::const_iterator kitRenameTo   = m_qstrUndoRenameTo.constBegin();
    while (kitRenameFrom != m_qstrUndoRenameFrom.constEnd())
    {
        idprgRenameProgress.UpdateMessage(tr("Renaming: %1\nTo: %2").arg(*kitRenameFrom).arg(*kitRenameTo));

        if (m_qdirDirReader.rename(*kitRenameFrom, *kitRenameTo) == false)
        {
            if (preldRenameErrorsDialog == nullptr)
                preldRenameErrorsDialog = new IDlgRenameErrorList(this);

            DetermineReasonForFailure(*kitRenameFrom, *kitRenameTo, qstrFailureReason);
            preldRenameErrorsDialog->AddFileToList(*kitRenameFrom, *kitRenameTo, qstrFailureReason);
        }

        ++kitRenameFrom;
        ++kitRenameTo;

        idprgRenameProgress.IncrementAndUpdateProgress();
        if (idprgRenameProgress.Aborted())
            break;
    }

    // The next control after the Undo button in the tab order is the address bar, which gets activated when the Undo button is deactivated.
    // This felt a bit odd, particularly when all the text was selected in the address bar, so we set focus to Name tab before deactivating Undo.
    m_qstrUndoRenameFrom.clear();
    m_qstrUndoRenameTo.clear();
    m_rpuirsRenameSettingsUI->m_pqtwRenameSettingsTab->setFocus();
    m_rpuirsRenameSettingsUI->m_pqpbUndoButton->setEnabled(false);

    RefreshDirectoryHard();
}


void IUIFileList::SetAutoRefreshEnabled(const bool kbAutoRefresh)
{
    if (m_bAutoRefresh != kbAutoRefresh)
    {
        m_bAutoRefresh = kbAutoRefresh;
        if (m_bAutoRefresh)
            RefreshDirectoryHard();
    }
}


int IUIFileList::GetCurrentRowHeight()
{
    return m_pqtwNameCurrent->verticalHeader()->defaultSectionSize();
}


void IUIFileList::SetUserDefinedRowHeight(const bool kbUseUserDefinedRowHeight, const int kiRowHeight)
{
    if (m_bUseUserDefinedRowHeight != kbUseUserDefinedRowHeight)
        m_bUseUserDefinedRowHeight = kbUseUserDefinedRowHeight;

    if (m_iUserDefinedRowHeight != kiRowHeight)
        m_iUserDefinedRowHeight = kiRowHeight;
}


/* There's a QTableWidget::resizeColumnsToContents() function, but it leaves a lot of padding.
 * I've tried to remove the padding with the below, but it doesn't reduce the padding.
 *      rpqtwTable->setStyleSheet("QTableWidget::item { padding: 0px; }");
 *      rpqtwTable->setStyleSheet("QTableWidget::item { padding: 0px; border: 0px; margin: 0px; }");
 * Using table->verticalHeader()->setDefaultSectionSize() seems to be the only way to remove the excessive white space in each row.
 *
 * There are similar problem with excessive padding between the icon and the file name that makes it look like there's a space at the start of the file name.
 * I tried the below, but once again it doesn't reduce the padding.
 *      rpqtwTable->setStyleSheet("QTableWidget::icon { padding: 0px; }");
 *      rpqtwTable->setStyleSheet("QTableWidget::icon { padding: 0px; border: 0px; margin: 0px; }");
 * I hate excessive padding and white space, but Qt doesn't give you much control over it. */
void  IUIFileList::SetRowHeightAndIconSize()
{
    int iRowHeight;
    if (m_bUseUserDefinedRowHeight)
        iRowHeight = m_iUserDefinedRowHeight;
    else
        iRowHeight = IComUtilityFuncs::GetTableRowHeightFitToFont(m_pqtwNameCurrent);

    if (iRowHeight == m_pqtwNameCurrent->verticalHeader()->defaultSectionSize())
        return;

    m_pqtwNameCurrent->verticalHeader()->setDefaultSectionSize(iRowHeight);
    m_pqtwNamePreview->verticalHeader()->setDefaultSectionSize(iRowHeight);

    int iIconSize = iRowHeight - (iRowHeight%16);
    QSize qsizIconSize(iIconSize, iIconSize);
    m_pqtwNameCurrent->setIconSize(qsizIconSize);
    m_pqtwNamePreview->setIconSize(qsizIconSize);
}


void IUIFileList::UpdateChangeHighlightSettings(const bool kbNameChangeColourText, const QColor & krqcolNameChangeTextColour, const bool kbNameChangeHighlightRow, const QColor & krqcolNameChangeHighlightColour)
{
    m_bNameChangeColourText         = kbNameChangeColourText;
    m_qcolNameChangeTextColour      = krqcolNameChangeTextColour;
    m_bNameChangeHighlightRow       = kbNameChangeHighlightRow;
    m_qcolNameChangeHighlightColour = krqcolNameChangeHighlightColour;

    HighlightRowsWithModifiedNames(true);
}


const QFont & IUIFileList::GetCurrentFont()
{
    return m_pqtwNameCurrent->font();
}


void IUIFileList::RestoreDefaultFonts()
{
    SetTableFont(m_qfntDefaultFont);
    //if (m_bUseUserDefinedRowHeight == false)
    //    SetRowHeightAndIconSize();
}


void IUIFileList::SetTableFont(const QFont & krqfntNewFont)
{
    if (krqfntNewFont != m_pqtwNameCurrent->font())
    {
        m_pqtwNameCurrent->setFont(krqfntNewFont);
        m_pqtwNamePreview->setFont(krqfntNewFont);
        //if (m_bUseUserDefinedRowHeight == false)
        //    SetRowHeightAndIconSize();
    }
}


void IUIFileList::keyPressEvent(QKeyEvent* pqkeEvent)
{
    if (pqkeEvent->key() == Qt::Key_F2)
        OpenRenameFileDlgForCurrentRow();
    else
        pqkeEvent->ignore();
}
