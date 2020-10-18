#include <QtWidgets>
#include <QDirIterator>
#include "IUIExtract.h"
#include "IUIMainWindow.h"
#include "IDlgExtractProgress.h"
#include "IMKVExtractProcess.h"
#include "IComSysMKVToolNix.h"
#include "IComUtilityFuncs.h"


IUIExtract::IUIExtract(IUIMainWindow* pmwMainWindow) : QWidget(pmwMainWindow)
{
    m_pmwMainWindow = pmwMainWindow;
    m_pimkvMKVToolNix = new IComSysMKVToolNix(pmwMainWindow);
    m_pdepExtractionProgress = nullptr;
    m_bFileBeingProcessed = false;
    m_bAttachmentsGroupModified = false;
    m_bDetectCuesheetsTags = m_pmwMainWindow->GetSettings().value("MKVToolNix/DetectCuesheetsTags", false).toBool();
    m_pkepMKVExtractor = nullptr;
    setAcceptDrops(true);
    m_qcolCollator.setNumericMode(true);

    QLabel *pqlblFileList, *pqlblBatchExtract, *pqlblOutputDir;
    QGroupBox* pqgbExtractForSelectedTracks;
    pqlblFileList = new QLabel(tr("Files List"));
    m_pqtwFileTree = new QTreeWidget(this);
    pqlblBatchExtract = new QLabel(tr("Batch Extract"));
    m_pqlwBatchExtractList = new QListWidget(this);
    m_pqcbExtractTimestamps = new QCheckBox(tr("Extract timestamps"), this);
    pqgbExtractForSelectedTracks = new QGroupBox("For selected tracks:", this);
    m_pqcbExtractCues = new QCheckBox(tr("Extract cues"), this);
    pqlblOutputDir = new QLabel(tr("Output Directory:"));
    m_pqleOutputDir = new QLineEdit(this);
    m_pqpbOutputDirSelect = new QPushButton("", this);
    m_pqpbClearList = new QPushButton(tr("Clear List"), this);
    m_pqpbBegin = new QPushButton(tr("Begin"), this);

    m_pqtwFileTree->header()->hide();
    //m_pqtwFileTree->setSortingEnabled(true);
    //m_pqtwFileTree->header()->setSortIndicator(0, Qt::AscendingOrder);

    m_pqpbClearList->setEnabled(false);
    m_pqpbBegin->setEnabled(false);

    m_pqpbOutputDirSelect->setIcon(pmwMainWindow->style()->standardIcon(QStyle::SP_DirOpenIcon));
    m_pqleOutputDir->setPlaceholderText(tr("Leave blank to extract to source directory"));
    m_pqpbOutputDirSelect->setFixedSize(m_pqleOutputDir->geometry().height(), m_pqleOutputDir->geometry().height());
    #ifdef Q_OS_WIN
    m_pqleOutputDir->setValidator(new QRegExpValidator(QRegExp("^[^/*?<>|]*$"), this));
    #endif

    QHBoxLayout* pqhblOutputDirLayout = new QHBoxLayout;
    pqhblOutputDirLayout->addWidget(pqlblOutputDir);
    pqhblOutputDirLayout->addWidget(m_pqleOutputDir);
    pqhblOutputDirLayout->addWidget(m_pqpbOutputDirSelect);

    QVBoxLayout* pqvblLeftLayout = new QVBoxLayout;
    pqvblLeftLayout->addWidget(pqlblFileList);
    pqvblLeftLayout->addWidget(m_pqtwFileTree);
    pqvblLeftLayout->addLayout(pqhblOutputDirLayout);

    QVBoxLayout* pqvblExtractForSelectedTracks = new QVBoxLayout;
    pqvblExtractForSelectedTracks->addWidget(m_pqcbExtractTimestamps);
    pqvblExtractForSelectedTracks->addWidget(m_pqcbExtractCues);
    pqgbExtractForSelectedTracks->setLayout(pqvblExtractForSelectedTracks);

    QVBoxLayout* pqvblRightLayout = new QVBoxLayout;
    pqvblRightLayout->addWidget(pqlblBatchExtract);
    pqvblRightLayout->addWidget(m_pqlwBatchExtractList);
    pqvblRightLayout->addWidget(pqgbExtractForSelectedTracks);
    pqvblRightLayout->addStretch();
    pqvblRightLayout->addWidget(m_pqpbClearList);
    pqvblRightLayout->addWidget(m_pqpbBegin);

    QHBoxLayout* pqhblMainLayout = new QHBoxLayout;
    pqhblMainLayout->addLayout(pqvblLeftLayout);
    pqhblMainLayout->addLayout(pqvblRightLayout);
    pqhblMainLayout->setStretchFactor(pqvblLeftLayout, 3);
    pqhblMainLayout->setStretchFactor(pqvblRightLayout, 1);
    setLayout(pqhblMainLayout);

    connect(&m_qprocMKVToolNix,     SIGNAL(readyReadStandardOutput()),             this, SLOT(MKVToolNixOutputText()));
    connect(&m_qprocMKVToolNix,     SIGNAL(readyReadStandardError()),              this, SLOT(MKVToolNixErrorText()));
    connect(&m_qprocMKVToolNix,     SIGNAL(finished(int, QProcess::ExitStatus)),   this, SLOT(MKVToolNixFinished(int, QProcess::ExitStatus)));
    connect(&m_qprocMKVToolNix,     SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(MKVToolNixError(QProcess::ProcessError)));

    connect(m_pqtwFileTree,         SIGNAL(itemChanged(QTreeWidgetItem*, int)),    this, SLOT(FileTreeItemToggled(QTreeWidgetItem*)));
    connect(m_pqlwBatchExtractList, SIGNAL(itemChanged(QListWidgetItem*)),         this, SLOT(SetBeginEnableState()));

    connect(m_pqpbOutputDirSelect,  SIGNAL(clicked()),                             this, SLOT(SelectOutputDirectory()));
    connect(m_pqpbClearList,        SIGNAL(clicked()),                             this, SLOT(ClearList()));
    connect(m_pqpbBegin,            SIGNAL(clicked()),                             this, SLOT(BeginExtraction()));

    #ifdef Q_OS_MACOS
    m_qprocMKVToolNix.setEnvironment(QStringList("LANG=en_US.UTF-8"));
    #endif
}


void IUIExtract::ProcessCommandLineParameters()
{
    QString qstrExtension;
    QStringList qstrlArguments = QCoreApplication::arguments();
    int iNumArgs = qstrlArguments.size();
    for (int iArgIndex = 1 ; iArgIndex < iNumArgs ; ++iArgIndex)
    {
        // File could be entered relative to the working directory so convert to full path
        if (QFileInfo::exists(qstrlArguments.at(iArgIndex)))
        {
            qstrExtension = qstrlArguments.at(iArgIndex).right(4).toLower();
            if (qstrExtension == ".mkv" || qstrExtension == ".mka")
            {
                QFileInfo qfiFile(qstrlArguments.at(iArgIndex));
                m_qstrlFileQueue.push_back(qfiFile.absoluteFilePath());
            }
        }
    }
    if (m_qstrlFileQueue.isEmpty() == false)
        ProcessNextFileInQueue();
}


void IUIExtract::AddToBatchExtractList(const QString & krqstrText, const int kiType)
{
    QListWidgetItem* pqlwiNewItem = new QListWidgetItem(nullptr, kiType);
    pqlwiNewItem->setText(krqstrText);
    pqlwiNewItem->setCheckState(Qt::Unchecked);

    int iNumItems = m_pqlwBatchExtractList->count();
    for (int iRow = 0 ; iRow < iNumItems ; ++iRow)
    {
        if (kiType < m_pqlwBatchExtractList->item(iRow)->type())
        {
            m_pqlwBatchExtractList->insertItem(iRow, pqlwiNewItem);
            return;
        }
    }
    m_pqlwBatchExtractList->addItem(pqlwiNewItem);
}


void IUIExtract::dragEnterEvent(QDragEnterEvent* pqdragEvent)
{
    if (pqdragEvent->mimeData()->hasFormat("text/uri-list"))
        pqdragEvent->acceptProposedAction();
}


void IUIExtract::dropEvent(QDropEvent* pqdropEvent)
{
    if (m_pimkvMKVToolNix->StoredMKVToolNixPathValid() == false)
    {
        m_pimkvMKVToolNix->ShowMsgRequiresMKVToolNix();
        return;
    }

    QList<QUrl> qlstqurlURLs = pqdropEvent->mimeData()->urls();
    if (qlstqurlURLs.isEmpty())
        return;

    QString qstrFilePath;
    QString qstrExtension;
    QList<QUrl>::ConstIterator kitURL;
    for (kitURL = qlstqurlURLs.constBegin() ; kitURL != qlstqurlURLs.constEnd() ; ++kitURL)
    {
        qstrFilePath = kitURL->toLocalFile();
        qstrExtension = qstrFilePath.right(4).toLower();
        if (qstrFilePath.isEmpty() == false && (qstrExtension == ".mkv" || qstrExtension == ".mka"))
            m_qstrlFileQueue.push_back(qstrFilePath);
    }
    ProcessNextFileInQueue();
}


void IUIExtract::OpenFilesDialog()
{
    if (m_pimkvMKVToolNix->StoredMKVToolNixPathValid() == false)
    {
        m_pimkvMKVToolNix->ShowMsgRequiresMKVToolNix();
        return;
    }

    QFileDialog qfdFileDlg(this);
    qfdFileDlg.setFileMode(QFileDialog::ExistingFiles);
    qfdFileDlg.setNameFilter("*.mkv *.mka");
    qfdFileDlg.setWindowTitle(tr("Select Files To Add"));

    QTreeWidgetItem* ptwiRootItem = m_pqtwFileTree->invisibleRootItem();
    if (ptwiRootItem->childCount() == 0)
    {
        #ifdef Q_OS_WIN
        qfdFileDlg.setDirectoryUrl(IComUtilityFuncs::GetMyComputerURL());
        #else
        qfdFileDlg.setDirectory(QDir::homePath());
        #endif
    }
    else
    {
        QString qstrStartDirectory = QDir::fromNativeSeparators(ptwiRootItem->child(0)->data(0, FilePathRoll).toString());
        qstrStartDirectory = qstrStartDirectory.left(qstrStartDirectory.lastIndexOf("/"));
        qfdFileDlg.setDirectory(qstrStartDirectory);
    }

    if (qfdFileDlg.exec())
    {
        QStringList qstrlFileList = qfdFileDlg.selectedFiles();
        QStringList::ConstIterator kitFile;
        for (kitFile = qstrlFileList.constBegin() ; kitFile != qstrlFileList.constEnd() ; ++kitFile)
        {
            if (kitFile->isEmpty() == false)
                m_qstrlFileQueue.push_back(*kitFile);
        }
        ProcessNextFileInQueue();
    }
}


/* Quzar - Open all files in a dir */
void IUIExtract::OpenDirDialog()
{
    if (m_pimkvMKVToolNix->StoredMKVToolNixPathValid() == false)
    {
        m_pimkvMKVToolNix->ShowMsgRequiresMKVToolNix();
        return;
    }

    QFileDialog qfdFileDlg(this);
    qfdFileDlg.setFileMode(QFileDialog::Directory);
    qfdFileDlg.setOption(QFileDialog::ShowDirsOnly, true);
    qfdFileDlg.setWindowTitle(tr("Select Directory To Scan"));

    QTreeWidgetItem* ptwiRootItem = m_pqtwFileTree->invisibleRootItem();
    if (ptwiRootItem->childCount() == 0)
    {
        #ifdef Q_OS_WIN
        qfdFileDlg.setDirectoryUrl(IComUtilityFuncs::GetMyComputerURL());
        #else
        qfdFileDlg.setDirectory(QDir::homePath());
        #endif
    }
    else
    {
        QString qstrStartDirectory = QDir::fromNativeSeparators(ptwiRootItem->child(0)->data(0, FilePathRoll).toString());
        qstrStartDirectory = qstrStartDirectory.left(qstrStartDirectory.lastIndexOf("/"));
        qfdFileDlg.setDirectory(qstrStartDirectory);
    }

    if (qfdFileDlg.exec())
    {
        /* Iterate through the chosen dir for all mkv files, then pass that list as if the list that a dialog box would make */
        QStringList filter = {"*.mkv", "*.mka"};
        QDirIterator it(qfdFileDlg.selectedFiles().constFirst(), filter, QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        QStringList qstrlFileList;
        while (it.hasNext())
            qstrlFileList << it.next();


        QStringList::ConstIterator kitFile;
        for (kitFile = qstrlFileList.constBegin() ; kitFile != qstrlFileList.constEnd() ; ++kitFile)
        {
            if (kitFile->isEmpty() == false)
                m_qstrlFileQueue.push_back(*kitFile);
        }
        ProcessNextFileInQueue();
    }
}


void IUIExtract::ProcessNextFileInQueue()
{ 
    if (m_bFileBeingProcessed || m_qstrlFileQueue.isEmpty())
        return;
    m_bFileBeingProcessed = true;

    QString qstrFilePath = m_qstrlFileQueue.at(0);
    QString qstrFileName = qstrFilePath.mid(qstrFilePath.lastIndexOf("/")+1);
    QTreeWidgetItem* ptwiRootItem = m_pqtwFileTree->invisibleRootItem();

    int iIndex;
    int iChildCount = ptwiRootItem->childCount();
    for (iIndex = 0 ; iIndex < iChildCount ; ++iIndex)
    {
        // Check if file is already in the GUI
        if (qstrFileName == ptwiRootItem->child(iIndex)->text(0))
        {
            // File names match but confirm full path matches
            if (qstrFilePath == QDir::fromNativeSeparators(ptwiRootItem->child(iIndex)->data(0, FilePathRoll).toString()))
            {
                m_bFileBeingProcessed = false;
                m_qstrlFileQueue.pop_front();
                ProcessNextFileInQueue();
                return;
            }
        }

        if (m_qcolCollator.compare(qstrFileName, ptwiRootItem->child(iIndex)->text(0)) < 0)
            break;
    }
    m_pqtwiFileItem = new QTreeWidgetItem(static_cast<QTreeWidget*>(nullptr), QStringList(qstrFileName), File);
    m_pqtwiFileItem->setData(0, FilePathRoll, QDir::toNativeSeparators(qstrFilePath));
    ptwiRootItem->insertChild(iIndex, m_pqtwiFileItem);
    m_pqtwiAttatchmentsGroup = nullptr;
    m_pqpbClearList->setEnabled(true);

    m_iActiveProcess = MKVMerge;
    m_qbaMKVMergeOutput.clear();
    QStringList qstrlArguments;
    if (m_pimkvMKVToolNix->IdentifyJSONAvailable())
    {
        qstrlArguments.append("--gui-mode");
        qstrlArguments.append("--identification-format");
        qstrlArguments.append("json");
        qstrlArguments.append("--identify");
        qstrlArguments.append(QDir::toNativeSeparators(qstrFilePath));
        // qDebug() << "Reading elements from JSON output";
    }
    else
    {
        qstrlArguments.append("--gui-mode");
        qstrlArguments.append("--identify-verbose");
        qstrlArguments.append(QDir::toNativeSeparators(qstrFilePath));
        // qDebug() << "Reading elements from Verbose output";
    }

    m_qprocMKVToolNix.setProgram(m_pimkvMKVToolNix->GetMKVMergePath());
    m_qprocMKVToolNix.setArguments(qstrlArguments);
    m_qprocMKVToolNix.start();
}


void IUIExtract::ProcessMKVMergeOutput()
{
    m_finfCurrentFileInfo.Reset();
    m_tinfTrackInfo.Reset();
    m_ainfAttachmentInfo.Reset();
    m_cctiChaptCueshtTagInfo.Reset();

    if (m_pimkvMKVToolNix->IdentifyJSONAvailable())
        ProcessMKVMergeOutputJSON();
    else
        ProcessMKVMergeOutputVerbose();

    if (m_pqtwiAttatchmentsGroup != nullptr)
    {
        m_pqtwiAttatchmentsGroup->setText(0, tr("Attachments (%1)").arg(m_finfCurrentFileInfo.GetTrackCountForType(Attachment)));
        m_pqtwiAttatchmentsGroup->sortChildren(0, Qt::AscendingOrder);
    }

    m_qbaMKVMergeOutput.clear();
}


void IUIExtract::ProcessMKVMergeOutputJSON()
{
    QJsonDocument qjdocMKVMergeIdent = QJsonDocument::fromJson(m_qbaMKVMergeOutput);
    if (qjdocMKVMergeIdent.isNull())
    {
        QMessageBox::critical(this, "Error Reading File Elements", tr("Error reading information from the file:\n\n%1").arg(m_pqtwiFileItem->data(0, FilePathRoll).toString()), QMessageBox::Ok);
        return;
    }

    QJsonObject qjobjFileObject = qjdocMKVMergeIdent.object();

    QJsonArray qjrgTracks = qjobjFileObject.value("tracks").toArray();
    for (int iIndex = 0 ; iIndex < qjrgTracks.size() ; ++iIndex)
    {
        m_tinfTrackInfo.ReadTrackInfo(qjrgTracks.at(iIndex).toObject());
        AddTrackToTree();
    }

    if (m_cctiChaptCueshtTagInfo.ReadChapterInfo(qjobjFileObject))
        AddChaptersCuesheetTagsToTree(Chapters);

    QJsonArray qjrgAttachmentss = qjobjFileObject.value("attachments").toArray();
    for (int iIndex = 0 ; iIndex < qjrgAttachmentss.size() ; ++iIndex)
    {
        m_ainfAttachmentInfo.ReadAttachmentInfo(qjrgAttachmentss.at(iIndex).toObject());
        AddAttachmentToTree();
    }
}


void IUIExtract::ProcessMKVMergeOutputVerbose()
{
    QString qstrMKVToolNixOutput = QString::fromUtf8(m_qbaMKVMergeOutput);
    QStringList qstrlOutputLines = qstrMKVToolNixOutput.split('\n', QString::SkipEmptyParts);

    QStringList::ConstIterator kitLine;
    for (kitLine = qstrlOutputLines.constBegin() ; kitLine != qstrlOutputLines.constEnd() ; ++kitLine)
    {
        if (kitLine->startsWith("Track"))
        {
            m_tinfTrackInfo.ReadTrackInfo(*kitLine);
            AddTrackToTree();
        }
        else if (kitLine->startsWith("Attachment"))
        {
            m_ainfAttachmentInfo.ReadAttachmentInfo(*kitLine);
            AddAttachmentToTree();
        }
        else if (kitLine->startsWith("Chapters"))
        {
            m_cctiChaptCueshtTagInfo.ReadChapterInfo(*kitLine);
            AddChaptersCuesheetTagsToTree(Chapters);
        }
    }
}


void IUIExtract::InitReadCuesheetTags(const int kiActiveProcess)
{
    m_iActiveProcess = kiActiveProcess;

    // Prior to version 17, MKVExtract printed cuesheets and tags to standard output, but starting with version 17 it only outputs to files
    // So the cuesheets/tags can be processed in the same way on all versions we're using --redirect-output on version < 17 to output to temporary file
    // Temporary files are written to QStandardPaths::TempLocation, which on each platform is:
    // Windows  - C:/Users/<USER>/AppData/Local/Temp
    // Linux    - /tmp
    // Mac      - Random location (/var/folders/randomstuff)

    QString qstrSourceFile = m_qstrlFileQueue.at(0);
    m_qstrCuesheetTagTempFile = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + qstrSourceFile.mid(qstrSourceFile.lastIndexOf('/')) + (m_iActiveProcess == MKVExtractCuesheet ? ".cue" : ".xml");

    QStringList qstrlArguments;
    if (m_pimkvMKVToolNix->NewMKVExtractInterface())
    {
        // New Syntax:  mkvextract source-filename cuesheet [options] output-filename.cue
        // New Syntax:  mkvextract source-filename tags [options] output-filename.cue
        qstrlArguments.append(QDir::toNativeSeparators(qstrSourceFile));
        qstrlArguments.append(m_iActiveProcess == MKVExtractCuesheet ? "cuesheet" : "tags");
        qstrlArguments.append("--gui-mode");
        qstrlArguments.append(QDir::toNativeSeparators(m_qstrCuesheetTagTempFile));
    }
    else
    {
        // Old Syntax:  mkvextract cuesheet {source-filename} [options]
        // Old Syntax:  mkvextract tags {source-filename} [options]
        qstrlArguments.append(m_iActiveProcess == MKVExtractCuesheet ? "cuesheet" : "tags");
        qstrlArguments.append(QDir::toNativeSeparators(qstrSourceFile));
        qstrlArguments.append("--gui-mode");
        qstrlArguments.append("--redirect-output");
        qstrlArguments.append(QDir::toNativeSeparators(m_qstrCuesheetTagTempFile));
    }

    m_qprocMKVToolNix.setProgram(m_pimkvMKVToolNix->GetMKVExtractPath());
    m_qprocMKVToolNix.setArguments(qstrlArguments);
    m_qprocMKVToolNix.start();
}


void IUIExtract::ProcessCuesheetTags()
{
    if (QFile::exists(m_qstrCuesheetTagTempFile) == false)
        return;

    QFile qfilFile(m_qstrCuesheetTagTempFile);
    if (qfilFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream qtsrmInputStream(&qfilFile);

        if (m_iActiveProcess == MKVExtractCuesheet)
        {
            if (m_cctiChaptCueshtTagInfo.ProcessCuesheet(qtsrmInputStream))
                AddChaptersCuesheetTagsToTree(Cuesheet);
        }
        else //if (m_iActiveProcess == MKVExtractTags)
        {
            if (m_cctiChaptCueshtTagInfo.ProcessTags(qtsrmInputStream))
                AddChaptersCuesheetTagsToTree(Tags);
        }
    }
    qfilFile.close();

    if (QFile::remove(m_qstrCuesheetTagTempFile) == false)
    {
        QMessageBox::critical(this, "Error Removing File", tr("Error deleting temporary file.\nIf you see this message please report it on the forum.\nAlso, please manually delete the file:\n\n%1").arg(m_qstrCuesheetTagTempFile), QMessageBox::Ok);
        return;
    }
}


void IUIExtract::AddTrackToTree()
{   
    QString qstrDescription;
    int iTrackType = m_tinfTrackInfo.GenerateTrackDescription(m_finfCurrentFileInfo, qstrDescription);

    QTreeWidgetItem* pqtwiTrackItem = new QTreeWidgetItem(static_cast<QTreeWidget*>(nullptr), QStringList(qstrDescription), iTrackType);
    pqtwiTrackItem->setData(0, CodecIDRole, m_tinfTrackInfo.GetCodecID());
    pqtwiTrackItem->setData(0, TrackIDRole, m_tinfTrackInfo.GetTrackID());
    pqtwiTrackItem->setData(0, CodecSupportedRole, m_tinfTrackInfo.IsSupported());
    AddItemToTree(pqtwiTrackItem);

    if (m_tinfTrackInfo.IsSupported() == false)
    {
        pqtwiTrackItem->setFlags(pqtwiTrackItem->flags() & ~Qt::ItemIsUserCheckable);
        pqtwiTrackItem->setForeground(0, Qt::red);
        m_pqtwiFileItem->setForeground(0, Qt::red);
    }

    m_tinfTrackInfo.Reset();
}


void IUIExtract::AddAttachmentToTree()
{
    if (m_pqtwiAttatchmentsGroup == nullptr)
    {
        m_pqtwiAttatchmentsGroup = new QTreeWidgetItem(static_cast<QTreeWidget*>(nullptr), QStringList(tr("Attachments")), AttachmentsGroup);
        AddItemToTree(m_pqtwiAttatchmentsGroup);
    }

    QString qstrDescription;
    m_ainfAttachmentInfo.GenerateAttatchmentDescription(m_finfCurrentFileInfo, qstrDescription);

    QTreeWidgetItem* pqtwiAttechmentItem = new QTreeWidgetItem(static_cast<QTreeWidget*>(nullptr), QStringList(qstrDescription), Attachment);
    pqtwiAttechmentItem->setData(0, TrackIDRole, m_ainfAttachmentInfo.GetAttachmentID());
    pqtwiAttechmentItem->setCheckState(0, Qt::Unchecked);
    m_pqtwiAttatchmentsGroup->addChild(pqtwiAttechmentItem);

    m_ainfAttachmentInfo.Reset();
}


void IUIExtract::AddChaptersCuesheetTagsToTree(const int kiElementType)
{
    int iNumItems;
    QString qstrDescription;

    if (kiElementType == Chapters)
        iNumItems = m_cctiChaptCueshtTagInfo.GenerateChaptersDescription(m_finfCurrentFileInfo, qstrDescription);
    else if (kiElementType == Cuesheet)
        iNumItems = m_cctiChaptCueshtTagInfo.GenerateCuesheetDescription(m_finfCurrentFileInfo, qstrDescription);
    else //if (kiElementType == Tags)
        iNumItems = m_cctiChaptCueshtTagInfo.GenerateTagsDescription(m_finfCurrentFileInfo, qstrDescription);

    if (iNumItems != 0)
    {
        QTreeWidgetItem* pqtwiNewItem = new QTreeWidgetItem(static_cast<QTreeWidget*>(nullptr), QStringList(qstrDescription), kiElementType);
        AddItemToTree(pqtwiNewItem);
    }
}


void IUIExtract::AddItemToTree(QTreeWidgetItem* pqtwiItem)
{
    pqtwiItem->setCheckState(0, Qt::Unchecked);

    int iIndex;
    int iTrackType = pqtwiItem->type();
    int iChildCount = m_pqtwiFileItem->childCount();
    for (iIndex = 0 ; iIndex < iChildCount ; ++iIndex)
    {
        if (iTrackType < m_pqtwiFileItem->child(iIndex)->type())
            break;
    }
    m_pqtwiFileItem->insertChild(iIndex, pqtwiItem);
}


void IUIExtract::MKVToolNixOutputText()
{
    m_qbaMKVMergeOutput.append(m_qprocMKVToolNix.readAllStandardOutput());
}


void IUIExtract::MKVToolNixErrorText()
{
    QByteArray qbaData = m_qprocMKVToolNix.readAllStandardError();
    QString qstrErrorText = QString::fromUtf8(qbaData);

    QString qstrTitle = tr("%1 Error", "Used when MKVMerge/MKVExtract encounters an error").arg(m_iActiveProcess == MKVMerge ? "MKVMerge" : "MKVExtract");
    QMessageBox::critical(this, qstrTitle, tr("Error while reading %1 from the file:\n\n%2\n\n%3")
                          .arg(m_iActiveProcess == MKVMerge ? tr("track information") : m_iActiveProcess == MKVExtractCuesheet ? tr("cuesheet") : tr("tags"))
                          .arg(m_pqtwiFileItem->data(0, FilePathRoll).toString())
                          .arg(qstrErrorText), QMessageBox::Ok);
}


void IUIExtract::MKVToolNixFinished(const int kiExitCode, const QProcess::ExitStatus kqpesExitStatus)
{
    bool bProccessNextFile = false;
    if (kqpesExitStatus == QProcess::CrashExit)
    {
        QString qstrTitle = tr("%1 Crashed", "Used when MKVMerge/MKVExtract crashes").arg(m_iActiveProcess == MKVMerge ? "MKVMerge" : "MKVExtract");
        QMessageBox::critical(this, qstrTitle, tr("Crash while reading track information from the file:\n\n%1").arg(m_pqtwiFileItem->data(0, FilePathRoll).toString()), QMessageBox::Ok);
        bProccessNextFile = true;
    }
    else if (kiExitCode == 2)
    {
        QString qstrTitle = tr("%1 Error", "Used when MKVMerge/MKVExtract finishes with errors").arg(m_iActiveProcess == MKVMerge ? "MKVMerge" : "MKVExtract");
        QMessageBox::critical(this, qstrTitle, tr("An error occured which prevented reading of information from the file:\n\n%1").arg(m_pqtwiFileItem->data(0, FilePathRoll).toString()), QMessageBox::Ok);
        bProccessNextFile = true;
    }
    else // Success (kiExitCode == 1 actually means warnings but...er...well I'm sure it's fine)
    {
        if (m_iActiveProcess == MKVMerge)
        {
            ProcessMKVMergeOutput();
            if (m_bDetectCuesheetsTags)
            {
                InitReadCuesheetTags(MKVExtractCuesheet);
            }
            else
            {
                m_finfMaxTrackValues.UpdateFileMaxValues(m_finfCurrentFileInfo, this);
                bProccessNextFile = true;
            }
        }
        else if (m_iActiveProcess == MKVExtractCuesheet)
        {
            ProcessCuesheetTags();
            InitReadCuesheetTags(MKVExtractTags);
        }
        else if (m_iActiveProcess == MKVExtractTags)
        {
            ProcessCuesheetTags();
            m_finfMaxTrackValues.UpdateFileMaxValues(m_finfCurrentFileInfo, this);
            bProccessNextFile = true;
        }
    }

    if (bProccessNextFile)
    {
        m_bFileBeingProcessed = false;
        m_qstrlFileQueue.pop_front();
        ProcessNextFileInQueue();
    }
}


void IUIExtract::MKVToolNixError(const QProcess::ProcessError kqpeError)
{
    if (kqpeError == QProcess::FailedToStart)
    {
        QString qstrTitle = tr("%1 Could Not Be Started").arg(m_iActiveProcess == MKVMerge ? "MKVMerge" : "MKVExtract");
        QMessageBox::critical(this, qstrTitle, tr("Please ensure the MKVToolNix directory has been correctly specified."), QMessageBox::Ok);
    }
}


void IUIExtract::FileTreeItemToggled(QTreeWidgetItem* pqtwiItem)
{
    if (pqtwiItem->type() == AttachmentsGroup)
    {
        if (m_bAttachmentsGroupModified)
            return;

        Qt::CheckState qcsCheckState = pqtwiItem->checkState(0);
        int iChildCount = pqtwiItem->childCount();
        for (int iIndex = 0 ; iIndex < iChildCount ; ++iIndex)
             pqtwiItem->child(iIndex)->setCheckState(0, qcsCheckState);
    }
    else if (pqtwiItem->type() == Attachment)
    {
        bool bCheckedFound = false;
        bool bUncheckedFound = false;
        QTreeWidgetItem* pqtwiAttachmentGroup = pqtwiItem->parent();
        int iChildCount = pqtwiAttachmentGroup->childCount();
        for (int iIndex = 0 ; iIndex < iChildCount ; ++iIndex)
        {
            if (pqtwiAttachmentGroup->child(iIndex)->checkState(0) == Qt::Checked)
                bCheckedFound = true;
            else if (pqtwiAttachmentGroup->child(iIndex)->checkState(0) == Qt::Unchecked)
                bUncheckedFound = true;
        }

        m_bAttachmentsGroupModified = true;
        if (bCheckedFound && bUncheckedFound)
            pqtwiAttachmentGroup->setCheckState(0, Qt::PartiallyChecked);
        else if (bCheckedFound)
            pqtwiAttachmentGroup->setCheckState(0, Qt::Checked);
        else
            pqtwiAttachmentGroup->setCheckState(0, Qt::Unchecked);
        m_bAttachmentsGroupModified = false;
    }

    SetBeginEnableState();
}


void IUIExtract::SetBeginEnableState()
{
    int iNumItems = m_pqlwBatchExtractList->count();
    for (int iRow = 0 ; iRow < iNumItems ; ++iRow)
    {
        if (m_pqlwBatchExtractList->item(iRow)->checkState() == Qt::Checked)
        {
            m_pqpbBegin->setEnabled(true);
            return;
        }
    }

    if (CheckedItemFound(m_pqtwFileTree->invisibleRootItem()) == true)
    {
        m_pqpbBegin->setEnabled(true);
        return;
    }

    m_pqpbBegin->setEnabled(false);
}


bool IUIExtract::CheckedItemFound(QTreeWidgetItem* ptwiParentItem)
{
    int iChildCount = ptwiParentItem->childCount();
    for (int iIndex = 0 ; iIndex < iChildCount ; ++iIndex)
    {
        if (ptwiParentItem->child(iIndex)->checkState(0) == Qt::Checked)
            return true;

        if (ptwiParentItem->child(iIndex)->childCount() != 0)
        {
            if (CheckedItemFound(ptwiParentItem->child(iIndex)) == true)
                return true;
        }
    }
    return false;
}


void IUIExtract::SelectOutputDirectory()
{
    QFileDialog qfdFileDlg(this);
    qfdFileDlg.setFileMode(QFileDialog::Directory);
    qfdFileDlg.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    qfdFileDlg.setWindowTitle(tr("Select Ouptut Directory"));

    QTreeWidgetItem* ptwiRootItem = m_pqtwFileTree->invisibleRootItem();
    if (m_pqleOutputDir->text().isEmpty() == false && QFileInfo::exists(QDir::fromNativeSeparators(m_pqleOutputDir->text())))
    {
        qfdFileDlg.setDirectory(QDir::fromNativeSeparators(m_pqleOutputDir->text()));
    }
    else if (ptwiRootItem->childCount() == 0)
    {
        #ifdef Q_OS_WIN
        qfdFileDlg.setDirectoryUrl(IComUtilityFuncs::GetMyComputerURL());
        #else
        qfdFileDlg.setDirectory(QDir::homePath());
        #endif
    }
    else
    {
        QString qstrDirectory = QDir::fromNativeSeparators(ptwiRootItem->child(0)->data(0, FilePathRoll).toString());
        qstrDirectory = qstrDirectory.left(qstrDirectory.lastIndexOf("/"));
        qfdFileDlg.setDirectory(qstrDirectory);
    }

    if (qfdFileDlg.exec())
        m_pqleOutputDir->setText(QDir::toNativeSeparators(qfdFileDlg.selectedFiles().at(0)));
}


void IUIExtract::ClearList()
{
    m_pqtwFileTree->clear();
    m_pqlwBatchExtractList->clear();
    m_pqleOutputDir->clear();
    m_pqcbExtractTimestamps->setChecked(false);
    m_pqcbExtractCues->setChecked(false);
    m_finfMaxTrackValues.Reset();

    m_pqpbClearList->setEnabled(false);
    m_pqpbBegin->setEnabled(false);
}


void IUIExtract::BeginExtraction()
{
    if (m_pqleOutputDir->text().isEmpty() == false)
    {
        QDir qdirDir;
        QString qstrOutputPath = QDir::fromNativeSeparators(m_pqleOutputDir->text());

        if (qstrOutputPath.at(qstrOutputPath.length()-1) == '/')
            qstrOutputPath.truncate(qstrOutputPath.length()-1);
        if (ConfirmDirectoryExists(qdirDir, qstrOutputPath) == false)
        {
            QMessageBox::critical(this, tr("Unable To Create Output Directory"), tr("Unable to begin extraction becuase the specified output directory could not be created\n\n%1").arg(m_pqleOutputDir->text()), QMessageBox::Ok);
            return;
        }
    }

    m_pdepExtractionProgress = new IDlgExtractProgress(m_pmwMainWindow, m_pqtwFileTree->invisibleRootItem()->childCount());
    ExtractNextFileInTree();
}


void IUIExtract::FileComplete(QTreeWidgetItem* ptwiCompleteItem)
{
    disconnect(m_pkepMKVExtractor, SIGNAL(ExtractionComplete(QTreeWidgetItem*)), this, SLOT(FileComplete(QTreeWidgetItem*)));
    delete m_pkepMKVExtractor;
    delete ptwiCompleteItem;
    m_pkepMKVExtractor = nullptr;

    m_pdepExtractionProgress->IncrementFilesProcessed();
    ExtractNextFileInTree();
}


void IUIExtract::ExtractNextFileInTree()
{
    QTreeWidgetItem* ptwiRootItem = m_pqtwFileTree->invisibleRootItem();
    if (ptwiRootItem->childCount() == 0)
    {
        m_pdepExtractionProgress->SetComplete();
        ClearList();
        return;
    }

    m_pdepExtractionProgress->SetFileBeingProcessed(ptwiRootItem->child(0)->text(0));
    m_pkepMKVExtractor = new IMKVExtractProcess(this, ptwiRootItem->child(0));
    connect(m_pkepMKVExtractor, SIGNAL(ExtractionComplete(QTreeWidgetItem*)), this, SLOT(FileComplete(QTreeWidgetItem*)));
    m_pkepMKVExtractor->BeginExtract();
}


bool IUIExtract::ConfirmDirectoryExists(QDir & rqdirDir, const QString & krqstrPath)
{
    if (rqdirDir.exists(krqstrPath))
        return true;

    QString qstrPathOneDirUp = krqstrPath.left(krqstrPath.lastIndexOf('/'));
    if (ConfirmDirectoryExists(rqdirDir, qstrPathOneDirUp) == false)
        return false;

    return rqdirDir.mkdir(krqstrPath);
}


void IUIExtract::AbortCurrentExtraction()
{
    if (m_pkepMKVExtractor != nullptr)
    {
        m_pkepMKVExtractor->AbortExtraction();
        delete m_pkepMKVExtractor;
        m_pkepMKVExtractor = nullptr;
        ClearList();
    }
}


QString IUIExtract::GetOuptputPath()
{
    return m_pqleOutputDir->text();
}


bool IUIExtract::ExtractTimestamps()
{
    return m_pqcbExtractTimestamps->isChecked();
}


bool IUIExtract::ExtractCues()
{
    return m_pqcbExtractCues->isChecked();
}
