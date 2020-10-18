#include <QtWidgets>
#include "IUIStripper.h"
#include "IUIMainWindow.h"
#include "IComUtilityFuncs.h"
#ifdef Q_OS_WIN
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#endif


IUIStripper::IUIStripper(IUIMainWindow* pmwMainWindow) : QWidget(pmwMainWindow)
{
    m_pmwMainWindow = pmwMainWindow;
    setAcceptDrops(true);
    m_qcolCollator.setNumericMode(true);

    QLabel *pqlblFileList, *pqlblStyleList, *pqlblOutputDir;
    pqlblFileList = new QLabel(tr("Files Added"));
    m_pqlwFileList = new QListWidget(this);
    pqlblStyleList = new QLabel(tr("Styles"));
    m_pqlwStyleList = new QListWidget(this);
    m_qcbStripComments = new QCheckBox(tr("Strip Comments From Scripts"), this);
    m_pqpbClearList = new QPushButton(tr("Clear List"), this);
    m_pqpbBegin = new QPushButton(tr("Begin"), this);
    pqlblOutputDir = new QLabel(tr("Output Directory:"));
    m_pqleOutputDir = new QLineEdit(this);
    m_pqpbOutputDirSelect = new QPushButton("", this);

    m_pqpbClearList->setEnabled(false);
    m_pqpbBegin->setEnabled(false);
    m_pqpbClearList->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pqpbBegin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_qcbStripComments->setLayoutDirection(Qt::RightToLeft);

    m_pqpbOutputDirSelect->setIcon(pmwMainWindow->style()->standardIcon(QStyle::SP_DirOpenIcon));
    m_pqleOutputDir->setPlaceholderText(tr("Leave blank to output to source directory"));
    m_pqpbOutputDirSelect->setFixedSize(m_pqleOutputDir->geometry().height(), m_pqleOutputDir->geometry().height());
    #ifdef Q_OS_WIN
    m_pqleOutputDir->setValidator(new QRegExpValidator(QRegExp("^[^/*?<>|]*$"), this));
    #endif

    QHBoxLayout* pqhblStylesLabelLayout = new QHBoxLayout;
    pqhblStylesLabelLayout->addWidget(pqlblStyleList);
    pqhblStylesLabelLayout->addWidget(m_qcbStripComments);

    QHBoxLayout* pqhblOutputDirLayout = new QHBoxLayout;
    pqhblOutputDirLayout->addWidget(pqlblOutputDir);
    pqhblOutputDirLayout->addWidget(m_pqleOutputDir);
    pqhblOutputDirLayout->addWidget(m_pqpbOutputDirSelect);

    QHBoxLayout* pqhblButtonLayout = new QHBoxLayout;
    pqhblButtonLayout->addWidget(m_pqpbClearList);
    pqhblButtonLayout->addWidget(m_pqpbBegin);

    QVBoxLayout* pqvblMainLayout = new QVBoxLayout;
    pqvblMainLayout->addWidget(pqlblFileList);
    pqvblMainLayout->addWidget(m_pqlwFileList);
    pqvblMainLayout->addLayout(pqhblStylesLabelLayout);
    pqvblMainLayout->addWidget(m_pqlwStyleList);
    pqvblMainLayout->addLayout(pqhblOutputDirLayout);
    pqvblMainLayout->addLayout(pqhblButtonLayout);

    pqvblMainLayout->setStretchFactor(m_pqlwFileList, 1);
    pqvblMainLayout->setStretchFactor(m_pqlwStyleList, 5);
    setLayout(pqvblMainLayout);

    connect(m_pqpbClearList,        SIGNAL(clicked()),                      this, SLOT(ClearList()));
    connect(m_pqpbBegin,            SIGNAL(clicked()),                      this, SLOT(BeginStripping()));
    connect(m_qcbStripComments,     SIGNAL(toggled(bool)),                  this, SLOT(SetBeginButtonEnableState()));
    connect(m_pqlwStyleList,        SIGNAL(itemChanged(QListWidgetItem*)),  this, SLOT(SetBeginButtonEnableStateStyleItemChange(QListWidgetItem*)));
    connect(m_pqpbOutputDirSelect,  SIGNAL(clicked()),                      this, SLOT(SelectOutputDirectory()));

    QSettings & rqsetSettings = pmwMainWindow->GetSettings();
    rqsetSettings.beginGroup("StripSettings");
    m_bSaveFilesToSubFolder = rqsetSettings.value("SaveFilesToSubFolder", false).toBool();
    m_qcbStripComments->setChecked(rqsetSettings.value("StripCommentsDefaultEnabled", false).toBool());
    rqsetSettings.endGroup();

    QTimer::singleShot(0, this, SLOT(ProcessCommandLineParameters()));
}


void IUIStripper::ProcessCommandLineParameters()
{
    QStringList qstrlArguments = QCoreApplication::arguments();
    int iNumArgs = qstrlArguments.size();
    for (int iArgIndex = 1 ; iArgIndex < iNumArgs ; ++iArgIndex)
    {
        // File could be entered relative to the working directory so convert to full path
        if (QFileInfo::exists(qstrlArguments.at(iArgIndex)))
        {
            QFileInfo qfiFile(qstrlArguments.at(iArgIndex));
            AddFile(qfiFile.absoluteFilePath());
        }
    }
}


void IUIStripper::dragEnterEvent(QDragEnterEvent* pqdragEvent)
{
    if (pqdragEvent->mimeData()->hasFormat("text/uri-list"))
        pqdragEvent->acceptProposedAction();
}


void IUIStripper::dropEvent(QDropEvent* pqdropEvent)
{
    QList<QUrl> qlstqurlURLs = pqdropEvent->mimeData()->urls();
    if (qlstqurlURLs.isEmpty())
        return;

    QList<QUrl>::ConstIterator kitURL;
    for (kitURL = qlstqurlURLs.constBegin() ; kitURL != qlstqurlURLs.constEnd() ; ++kitURL)
        AddFile(kitURL->toLocalFile());
    SetBeginButtonEnableState();
}


void IUIStripper::AddFile(const QString & krqstrFilePath)
{
    if (QFileInfo::exists(krqstrFilePath) == false)
        return;

    QString qstrExtension = krqstrFilePath.right(4).toLower();
    if (krqstrFilePath.isEmpty() || (qstrExtension != ".ass" && qstrExtension != ".ssa"))
        return;

    int iRow, iCompare;
    int iItemCount = m_pqlwFileList->count();
    QString qstrNativeFilePath = QDir::toNativeSeparators(krqstrFilePath);
    for (iRow = 0 ; iRow < iItemCount ; ++iRow)
    {
        iCompare = m_qcolCollator.compare(qstrNativeFilePath, m_pqlwFileList->item(iRow)->text());
        if (iCompare == 0)
            return;
        else if (iCompare < 0)
            break;
    }

    QListWidgetItem* pqlwiFileItem = new QListWidgetItem(qstrNativeFilePath);
    pqlwiFileItem->setData(Qt::UserRole, krqstrFilePath);
    m_pqlwFileList->insertItem(iRow, pqlwiFileItem);
    ReadStyles(krqstrFilePath);
}


void IUIStripper::ReadStyles(const QString & krqstrFilePath)
{
    QFile qfilSubFile(krqstrFilePath);
    if (!qfilSubFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream qtsrmSubFile(&qfilSubFile);

    if (SeekToStyles(qtsrmSubFile) == false)
        return;

    ReadStyles(qtsrmSubFile);
}


bool IUIStripper::SeekToStyles(QTextStream & rqtsrmSubFile)
{
    QString strLine = rqtsrmSubFile.readLine();
    while (!strLine.isNull())
    {
        // ASS = [V4+ Styles], SSA = [V4 Styles]
        if (strLine.startsWith("[V4"))
            return true;
        strLine = rqtsrmSubFile.readLine();
    }
    return false;
}


void IUIStripper::ReadStyles(QTextStream & rqtsrmSubFile)
{
    QString strLine = rqtsrmSubFile.readLine();
    while (!strLine.isNull())
    {
        if (strLine.startsWith("Style:"))
            AddStyleToList(ExtractStyleName(strLine));
        else if (strLine.startsWith("["))
            return;
        strLine = rqtsrmSubFile.readLine();
    }
}


QString IUIStripper::ExtractStyleName(const QString & krqstrStyleLine)
{
    int iStartIndex = 6;
    while (krqstrStyleLine.at(iStartIndex).isSpace())
        ++iStartIndex;

    int iLength = krqstrStyleLine.indexOf(',') - iStartIndex;

    return krqstrStyleLine.mid(iStartIndex, iLength);
}


void IUIStripper::AddStyleToList(const QString & krqstrStyleName)
{
    int iRow, iCompare;
    int iItemCount = m_pqlwStyleList->count();
    for (iRow = 0 ; iRow < iItemCount ; ++iRow)
    {
        iCompare = m_qcolCollator.compare(krqstrStyleName, m_pqlwStyleList->item(iRow)->text());
        if (iCompare == 0)
            return;
        else if (iCompare < 0)
            break;
    }

    QListWidgetItem* pqlwiStyleItem = new QListWidgetItem(krqstrStyleName);
    pqlwiStyleItem->setCheckState(Qt::Unchecked);
    m_pqlwStyleList->insertItem(iRow, pqlwiStyleItem);
}


void IUIStripper::BeginStripping()
{
    if (m_pqleOutputDir->text().isEmpty() == false)
    {
        QDir qdirDir;
        QString qstrOutputPath = QDir::fromNativeSeparators(m_pqleOutputDir->text());

        if (qstrOutputPath.at(qstrOutputPath.length()-1) == '/')
            qstrOutputPath.truncate(qstrOutputPath.length()-1);
        if (ConfirmDirectoryExists(qdirDir, qstrOutputPath) == false)
        {
            QMessageBox::critical(this, tr("Unable To Create Output Directory"), tr("Unable to begin stripping becuase the specified output directory could not be created\n\n%1").arg(m_pqleOutputDir->text()), QMessageBox::Ok);
            return;
        }
    }

    GenerateStylesToRemoveList();

    const int kiNumItems = m_pqlwFileList->count();
    QProgressDialog qprgdStripProgress("Stripping Files", "Abort", 0, kiNumItems, this);
    qprgdStripProgress.setWindowFlags(qprgdStripProgress.windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);
    qprgdStripProgress.resize(400, qprgdStripProgress.sizeHint().height());
    qprgdStripProgress.setModal(true);

    #ifdef Q_OS_WIN
    QWinTaskbarButton* pqtbbTaskbarButton = new QWinTaskbarButton(&qprgdStripProgress);
    pqtbbTaskbarButton->setWindow(m_pmwMainWindow->windowHandle());
    QWinTaskbarProgress* m_pqtbpTaskbarProgress = pqtbbTaskbarButton->progress();
    m_pqtbpTaskbarProgress->setMaximum(kiNumItems);
    m_pqtbpTaskbarProgress->show();
    #endif

    QString qstrFile;
    for (int iRow = 0 ; iRow < kiNumItems ; ++iRow)
    {
        qstrFile = m_pqlwFileList->item(iRow)->data(Qt::UserRole).toString();
        qprgdStripProgress.setLabelText(tr("<h3>Striping In Progress</h3>Stripping file: %1").arg(qstrFile.mid(qstrFile.lastIndexOf('/')+1)));
        qprgdStripProgress.setValue(iRow);
        #ifdef Q_OS_WIN
        m_pqtbpTaskbarProgress->setValue(iRow);
        #endif

        StripFile(qstrFile);

        if (qprgdStripProgress.wasCanceled())
            break;
    }
    qprgdStripProgress.setValue(kiNumItems);
    #ifdef Q_OS_WIN
    m_pqtbpTaskbarProgress->hide();
    #endif

    m_qstrlStylesToRemovStyles.clear();
    m_qstrlStylesToRemovEvents.clear();    
    ClearList();
}


bool IUIStripper::ConfirmDirectoryExists(QDir & rqdirDir, const QString & krqstrPath)
{
    if (rqdirDir.exists(krqstrPath))
        return true;

    QString qstrPathOneDirUp = krqstrPath.left(krqstrPath.lastIndexOf('/'));
    if (ConfirmDirectoryExists(rqdirDir, qstrPathOneDirUp) == false)
        return false;

    return rqdirDir.mkdir(krqstrPath);
}


void IUIStripper::GenerateStylesToRemoveList()
{
    int iNumItems = m_pqlwStyleList->count();
    for (int iRow = 0 ; iRow < iNumItems ; ++iRow)
    {
        if (m_pqlwStyleList->item(iRow)->checkState() == Qt::Checked)
        {
            m_qstrlStylesToRemovStyles.push_back("Style: " + m_pqlwStyleList->item(iRow)->text() + ',');
            m_qstrlStylesToRemovEvents.push_back(m_pqlwStyleList->item(iRow)->text());
        }
    }
}


void IUIStripper::StripFile(const QString & krqstrFilePath)
{
    QString qstrOutputFilePath = GenerateOutputFileName(krqstrFilePath);
    if (qstrOutputFilePath.isEmpty())
        return;

    QFile qfilSubIn(krqstrFilePath);
    QFile qfilSubOut(qstrOutputFilePath);

    if (!qfilSubIn.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    if (!qfilSubOut.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream qtsrmSubIn(&qfilSubIn);
    QTextStream qtsrmSubOut(&qfilSubOut);
    bool bKeepComments = m_qcbStripComments->isChecked() == false;

    m_iStyleStartIndex = 1;
    int iCurrentlyReading = Header;
    QString strLine = qtsrmSubIn.readLine();
    while (!strLine.isNull())
    {
        if (iCurrentlyReading == Header)
        {
            if (strLine.startsWith("[V4"))
                iCurrentlyReading = Styles;
            if (strLine.contains("Active Line:") == false && strLine.contains("Scroll Position:") == false && strLine.contains("Video Position:") == false)
                qtsrmSubOut << strLine << endl;
        }
        else if (iCurrentlyReading == Styles)
        {
            if (strLine.startsWith("[Events]"))
                iCurrentlyReading = Events;
            if (KeepStyle(strLine))
                qtsrmSubOut << strLine << endl;
        }
        else //if (iCurrentlyReading == Events)
        {
            if (strLine.startsWith("Dialogue:"))
            {
                if (KeepEvent(strLine))
                    qtsrmSubOut << strLine << endl;
            }
            else if (strLine.startsWith("Comment:"))
            {
                if (bKeepComments)
                    qtsrmSubOut << strLine << endl;
            }
            else if (strLine.startsWith("Format:"))
            {
                qtsrmSubOut << strLine << endl;
            }
            else if (strLine.startsWith('['))
            {
                break;
            }
        }
        strLine = qtsrmSubIn.readLine();
    }

    qfilSubIn.close();
    qfilSubOut.close();
}


QString IUIStripper::GenerateOutputFileName(const QString & krqstrFilePath)
{
    if (m_pqleOutputDir->text().isEmpty() == false)
    {
        QString qstrOutputPath = QDir::fromNativeSeparators(m_pqleOutputDir->text());
        if (qstrOutputPath.at(qstrOutputPath.length()-1) == '/')
            qstrOutputPath.truncate(qstrOutputPath.length()-1);

        QString File = krqstrFilePath.left(krqstrFilePath.lastIndexOf('/'));
        if (krqstrFilePath.left(krqstrFilePath.lastIndexOf('/')) == qstrOutputPath)
        {
            // If the file path is the same as the specified output path appened Stripped to name to avoid conflict
            int iExtensionIndex = krqstrFilePath.lastIndexOf(".");
            return krqstrFilePath.left(iExtensionIndex) + "-Stripped" + krqstrFilePath.mid(iExtensionIndex);
        }
        else
        {
            return qstrOutputPath + krqstrFilePath.mid(krqstrFilePath.lastIndexOf("/"));
        }
    }
    else if (m_bSaveFilesToSubFolder)
    {
        QDir qdirDirectory;
        int iLastSlashIndex = krqstrFilePath.lastIndexOf("/");
        QString qstrSubDirectoryPath = krqstrFilePath.left(iLastSlashIndex) + "/Stripped";
        if (qdirDirectory.exists(qstrSubDirectoryPath) == false)
        {
            if (qdirDirectory.mkdir(qstrSubDirectoryPath) == false)
            {
                QMessageBox::critical(this, tr("Error Creating Output Directory"), tr("Unable to create output directory:\n\n%1").arg(qdirDirectory.toNativeSeparators(qstrSubDirectoryPath)), QMessageBox::Ok);
                return QString();
            }
        }
        return qstrSubDirectoryPath + krqstrFilePath.mid(iLastSlashIndex);
    }
    else
    {
        int iExtensionIndex = krqstrFilePath.lastIndexOf(".");
        return krqstrFilePath.left(iExtensionIndex) + "-Stripped" + krqstrFilePath.mid(iExtensionIndex);
    }
}


bool IUIStripper::KeepStyle(const QString & krqstrStyleLine)
{
    QStringList::ConstIterator kiStyle;
    for (kiStyle = m_qstrlStylesToRemovStyles.constBegin() ; kiStyle != m_qstrlStylesToRemovStyles.constEnd() ; ++kiStyle)
    {
        if (krqstrStyleLine.startsWith(*kiStyle))
            return false;
    }
    return true;
}


bool IUIStripper::KeepEvent(const QString & krqstrEventLine)
{
    if (krqstrEventLine.isEmpty())
        return false;

    if (krqstrEventLine.at(m_iStyleStartIndex-1) != ',')
        SetStyleStartIndex(krqstrEventLine);
    int iStyleEndIndex = GetStyleEndIndex(krqstrEventLine);
    QString qstrStyle = krqstrEventLine.mid(m_iStyleStartIndex, iStyleEndIndex-m_iStyleStartIndex);

    QStringList::ConstIterator kitStyle;
    for (kitStyle = m_qstrlStylesToRemovEvents.constBegin() ; kitStyle != m_qstrlStylesToRemovEvents.constEnd() ; ++kitStyle)
    {
        if (*kitStyle == qstrStyle)
            return false;
    }
    return true;
}


void IUIStripper::SetStyleStartIndex(const QString & krqstrEventLine)
{
    // Style starts after 3rd comma in both ASS and SSA
    int iCommaCount = 3;
    m_iStyleStartIndex = 0;
    while (iCommaCount > 0)
    {
        if (krqstrEventLine.at(m_iStyleStartIndex) == ',')
            --iCommaCount;
        ++m_iStyleStartIndex;
    }
}


int IUIStripper::GetStyleEndIndex(const QString & krqstrEventLine)
{
    int iIndex = m_iStyleStartIndex;
    while (krqstrEventLine.at(iIndex) != ',')
        ++iIndex;
    return iIndex;
}


void IUIStripper::SelectOutputDirectory()
{
    QFileDialog qfdFileDlg(this);
    qfdFileDlg.setFileMode(QFileDialog::Directory);
    qfdFileDlg.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    qfdFileDlg.setWindowTitle(tr("Select Ouptut Directory"));

    if (m_pqleOutputDir->text().isEmpty() == false && QFileInfo::exists(QDir::fromNativeSeparators(m_pqleOutputDir->text())))
    {
        qfdFileDlg.setDirectory(QDir::fromNativeSeparators(m_pqleOutputDir->text()));
    }
    else if (m_pqlwFileList->count() == 0)
    {
        #ifdef Q_OS_WIN
        qfdFileDlg.setDirectoryUrl(IComUtilityFuncs::GetMyComputerURL());
        #else
        qfdFileDlg.setDirectory(QDir::homePath());
        #endif
    }
    else
    {
        QString qstrDirectory = m_pqlwFileList->item(0)->data(Qt::UserRole).toString();
        qstrDirectory = qstrDirectory.left(qstrDirectory.lastIndexOf("/"));
        qfdFileDlg.setDirectory(qstrDirectory);
    }

    if (qfdFileDlg.exec())
        m_pqleOutputDir->setText(QDir::toNativeSeparators(qfdFileDlg.selectedFiles().at(0)));
}


void IUIStripper::ClearList()
{
    m_pqlwFileList->clear();
    m_pqlwStyleList->clear();
    m_pqleOutputDir->clear();

    m_pqpbClearList->setEnabled(false);
    m_pqpbBegin->setEnabled(false);
}


void IUIStripper::SetBeginButtonEnableState()
{
    if (m_pqlwFileList->count() == 0)
    {
        m_pqpbBegin->setEnabled(false);
    }
    else if (m_qcbStripComments->isChecked())
    {
        m_pqpbBegin->setEnabled(true);
    }
    else
    {
        int iNumItems = m_pqlwStyleList->count();
        for (int iRow = 0 ; iRow < iNumItems ; ++iRow)
        {
            if (m_pqlwStyleList->item(iRow)->checkState() == Qt::Checked)
            {
                m_pqpbBegin->setEnabled(true);
                return;
            }
        }
        m_pqpbBegin->setEnabled(false);
    }
}


void IUIStripper::SetBeginButtonEnableStateStyleItemChange(QListWidgetItem* pqlwiItem)
{
    if (pqlwiItem->checkState() == Qt::Checked)
         m_pqpbBegin->setEnabled(true);
    else
        SetBeginButtonEnableState();
}


void IUIStripper::OpenFilesDialog()
{
    QFileDialog qfdFileDlg(this);
    qfdFileDlg.setFileMode(QFileDialog::ExistingFiles);
    qfdFileDlg.setNameFilter("*.ass *.ssa");
    qfdFileDlg.setWindowTitle(tr("Select Files To Add"));

    if (m_pqlwFileList->count() == 0)
    {
        #ifdef Q_OS_WIN
        qfdFileDlg.setDirectoryUrl(IComUtilityFuncs::GetMyComputerURL());
        #else
        qfdFileDlg.setDirectory(QDir::homePath());
        #endif
    }
    else
    {
        QString qstrStartDirectory = m_pqlwFileList->item(0)->data(Qt::UserRole).toString();
        qstrStartDirectory = qstrStartDirectory.left(qstrStartDirectory.lastIndexOf("/"));
        qfdFileDlg.setDirectory(qstrStartDirectory);
    }

    if (qfdFileDlg.exec())
    {
        QStringList qstrlFileNames = qfdFileDlg.selectedFiles();
        QStringList::ConstIterator kitFile;
        for (kitFile = qstrlFileNames.constBegin() ; kitFile != qstrlFileNames.constEnd() ; ++kitFile)
        {
            if (kitFile->isEmpty() == false)
                AddFile(*kitFile);
        }
    }
}


void IUIStripper::SetStripCommentsCheckbox(const bool kbChecked)
{
    m_qcbStripComments->setChecked(kbChecked);
}
