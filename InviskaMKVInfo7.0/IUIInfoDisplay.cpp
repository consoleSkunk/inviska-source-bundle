#include <QtWidgets>
#include "IUIInfoDisplay.h"
#include "IUIMainWindow.h"
#include "IUIMenuBar.h"
#include "IDlgScanProgress.h"
#include "IComSysMKVToolNix.h"
#include "IComUtilityFuncs.h"


IUIInfoDisplay::IUIInfoDisplay(IUIMainWindow* pmwMainWindow) : QWidget(pmwMainWindow)
{
    m_pmwMainWindow = pmwMainWindow;
    m_pimkvMKVToolNix = new IComSysMKVToolNix(pmwMainWindow);
    m_pdspScanProgress = nullptr;
    m_bScanVerbose = false;
    m_iPhase = Inactive;
    setAcceptDrops(true);

    QSettings & rqsetSettings = m_pmwMainWindow->GetSettings();
    if (rqsetSettings.value("Application/RestoreScanLevel", false).toBool())
        m_bScanVerbose = rqsetSettings.value("MKVToolNix/ScanVerbose", false).toBool();

    QLabel* pqlblOpenFileLabel = new QLabel(tr("File: "), this);
    m_pqleOpenFilePath = new QLineEdit(this);
    m_pqpbOpenFile = new QPushButton("", this);
    m_pqleOpenFilePath->setReadOnly(true);
    m_pqpbOpenFile->setIcon(pmwMainWindow->style()->standardIcon(QStyle::SP_DirOpenIcon));
    m_pqpbOpenFile->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_pqtwInfoTree = new QTreeWidget(this);
    m_pqtwInfoTree->header()->hide();
    m_pqtwInfoTree->setStyleSheet("QTreeWidget {border: 0px;}");

    m_pqpteInfoText = new QPlainTextEdit(this);
    m_pqpteInfoText->setReadOnly(true);
    m_pqpteInfoText->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_pqpteInfoText->setStyleSheet("QPlainTextEdit {border: 0px;}");

    m_pqpteMergeText = new QPlainTextEdit(this);
    m_pqpteMergeText->setReadOnly(true);
    m_pqpteMergeText->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_pqpteMergeText->setStyleSheet("QPlainTextEdit {border: 0px;}");

    m_pqtwTabWidget = new QTabWidget(this);
    m_pqtwTabWidget->addTab(m_pqtwInfoTree, tr("MKVInfo &Tree"));
    m_pqtwTabWidget->addTab(m_pqpteInfoText, tr("MKVInfo Te&xt"));
    m_pqtwTabWidget->addTab(m_pqpteMergeText, tr("MKVMerge &Identify"));
    m_pqtwTabWidget->setCurrentIndex(rqsetSettings.value("Application/DefaultTab", MKVInfoTree).toInt());


    m_pqpbSaveInfo  = new QPushButton(tr("&Save MKVInfo Output"), this);
    m_pqpbSaveMerge = new QPushButton(tr("&Save MKVMerge Ouptut"), this);
    m_pqpbClear     = new QPushButton(tr("&Clear Ouptut"), this);
    m_pqpbSaveInfo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pqpbSaveMerge->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pqpbClear->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pqpbSaveInfo->setMinimumWidth(m_pqpbSaveMerge->sizeHint().width());
    m_pqpbClear->setMinimumWidth(m_pqpbSaveMerge->sizeHint().width());
    m_pqpbSaveInfo->setEnabled(false);
    m_pqpbSaveMerge->setEnabled(false);
    m_pqpbClear->setEnabled(false);

    QHBoxLayout* pqhblCurrentFileLayout = new QHBoxLayout;
    pqhblCurrentFileLayout->addWidget(pqlblOpenFileLabel);
    pqhblCurrentFileLayout->addWidget(m_pqleOpenFilePath);
    pqhblCurrentFileLayout->addWidget(m_pqpbOpenFile);

    QHBoxLayout* pqhblButtonLayout = new QHBoxLayout;
    pqhblButtonLayout->addWidget(m_pqpbSaveInfo);
    pqhblButtonLayout->addWidget(m_pqpbSaveMerge);
    pqhblButtonLayout->addWidget(m_pqpbClear);

    QVBoxLayout* pqvblLayout = new QVBoxLayout;
    pqvblLayout->addLayout(pqhblCurrentFileLayout);
    pqvblLayout->addWidget(m_pqtwTabWidget);
    pqvblLayout->addLayout(pqhblButtonLayout);
    setLayout(pqvblLayout);

    connect(m_pqpbOpenFile,         SIGNAL(clicked()),                             this, SLOT(OpenFileDialog()));
    connect(m_pqpbSaveInfo,         SIGNAL(clicked()),                             this, SLOT(SaveMKVInfoOutput()));
    connect(m_pqpbSaveMerge,        SIGNAL(clicked()),                             this, SLOT(SaveMKVMergeOutput()));
    connect(m_pqpbClear,            SIGNAL(clicked()),                             this, SLOT(Clear()));

    connect(&m_qprocMKVToolNix,     SIGNAL(readyReadStandardOutput()),             this, SLOT(MKVToolNixOutputText()));
    connect(&m_qprocMKVToolNix,     SIGNAL(readyReadStandardError()),              this, SLOT(MKVToolNixErrorText()));
    connect(&m_qprocMKVToolNix,     SIGNAL(finished(int, QProcess::ExitStatus)),   this, SLOT(MKVToolNixFinished(int, QProcess::ExitStatus)));
    connect(&m_qprocMKVToolNix,     SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(MKVToolNixError(QProcess::ProcessError)));

    #ifdef Q_OS_MACOS
    m_qprocMKVToolNix.setEnvironment(QStringList("LANG=en_US.UTF-8"));
    #endif
}


void IUIInfoDisplay::ProcessCommandLineParameters()
{
    QStringList qstrlArguments = QCoreApplication::arguments();
    int iNumArgs = qstrlArguments.size();
    for (int iArgIndex = 1 ; iArgIndex < iNumArgs ; ++iArgIndex)
    {
        // File could be entered relative to the working directory so convert to full path
        if (QFileInfo::exists(qstrlArguments.at(iArgIndex)))
        {
            QFileInfo qfiFile(qstrlArguments.at(iArgIndex));
            if (OpenFile(qfiFile.absoluteFilePath()) == true)
                break;
        }
    }
}


void IUIInfoDisplay::dragEnterEvent(QDragEnterEvent* pqdragEvent)
{
    if (pqdragEvent->mimeData()->hasFormat("text/uri-list"))
        pqdragEvent->acceptProposedAction();
}


void IUIInfoDisplay::dropEvent(QDropEvent* pqdropEvent)
{
    if (m_pimkvMKVToolNix->StoredMKVToolNixPathValid() == false)
    {
        m_pimkvMKVToolNix->ShowMsgRequiresMKVToolNix();
        return;
    }

    QList<QUrl> qlstqurlURLs = pqdropEvent->mimeData()->urls();
    if (qlstqurlURLs.isEmpty())
        return;

    QList<QUrl>::ConstIterator kitURL;
    for (kitURL = qlstqurlURLs.constBegin() ; kitURL != qlstqurlURLs.constEnd() ; ++kitURL)
    {
        if (OpenFile(kitURL->toLocalFile()) == true)
            break;
    }
}


void IUIInfoDisplay::OpenFileDialog()
{
    if (m_pimkvMKVToolNix->StoredMKVToolNixPathValid() == false)
    {
        m_pimkvMKVToolNix->ShowMsgRequiresMKVToolNix();
        return;
    }

    QFileDialog qfdFileDlg(this);
    qfdFileDlg.setFileMode(QFileDialog::ExistingFile);
    qfdFileDlg.setNameFilter("*.mkv *.mka");
    qfdFileDlg.setWindowTitle(tr("Open File"));

    if (m_qstrOpenFilePath.isEmpty())
    {
        #ifdef Q_OS_WIN
        qfdFileDlg.setDirectoryUrl(IComUtilityFuncs::GetMyComputerURL());
        #else
        qfdFileDlg.setDirectory(QDir::homePath());
        #endif
    }
    else
    {
        qfdFileDlg.setDirectory(m_qstrOpenFilePath.left(m_qstrOpenFilePath.lastIndexOf("/")+1));
    }

    if (qfdFileDlg.exec())
        OpenFile(qfdFileDlg.selectedFiles().at(0));
}


bool IUIInfoDisplay::OpenFile(const QString & krqstrFilePath)
{
    if (m_iPhase != Inactive)
        return false;
    m_iPhase = Initialising;

    if (QFileInfo::exists(krqstrFilePath) == false)
    {
        m_iPhase = Inactive;
        return false;
    }

    QString qstrExtension = krqstrFilePath.right(4).toLower();
    if (qstrExtension != ".mkv" && qstrExtension != ".mka")
    {
        m_iPhase = Inactive;
        return false;
    }

    m_qstrOpenFilePath = krqstrFilePath;
    m_pqleOpenFilePath->setText(QDir::toNativeSeparators(krqstrFilePath));
    InitVariables();

    if (m_bScanVerbose)
        m_pdspScanProgress = new IDlgScanProgress(m_pmwMainWindow, this, krqstrFilePath);

    InitMKVMerge();
    return true;
}


void IUIInfoDisplay::InitVariables()
{
    m_iCurrentDepth = 0;
    m_pqtwiParent = m_pqtwInfoTree->invisibleRootItem();
    m_pqtwiLastItem = m_pqtwInfoTree->invisibleRootItem();
    m_pqtwInfoTree->clear();
    m_pqpteInfoText->clear();
    m_pqpteMergeText->clear();
    m_qstrPartialLine.clear();
}


void IUIInfoDisplay::InitMKVMerge()
{
    m_iPhase = MKVMerge;

    QStringList qstrlArguments;
    if (m_pimkvMKVToolNix->IdentifyJSONAvailable())
    {
        // mkvmerge --identification-format json --identify file.mkv
        qstrlArguments.append("--gui-mode");
        qstrlArguments.append("--identification-format");
        qstrlArguments.append("json");
        qstrlArguments.append("--identify");
        qstrlArguments.push_back(QDir::toNativeSeparators(m_qstrOpenFilePath));
    }
    else
    {
        qstrlArguments.append("--gui-mode");
        qstrlArguments.append("--identify-verbose");
        qstrlArguments.push_back(QDir::toNativeSeparators(m_qstrOpenFilePath));
    }

    m_qprocMKVToolNix.setProgram(m_pimkvMKVToolNix->GetMKVMergePath());
    m_qprocMKVToolNix.setArguments(qstrlArguments);
    m_qprocMKVToolNix.start();
}


void IUIInfoDisplay::InitMKVInfo()
{
    m_iPhase = MKVInfo;

    QStringList qstrlArguments;
    qstrlArguments.append("--gui-mode");
    if (m_bScanVerbose)
    {
        qstrlArguments.append("-v");
        qstrlArguments.append("-v");
    }
    qstrlArguments.append(QDir::toNativeSeparators(m_qstrOpenFilePath));

    m_qprocMKVToolNix.setProgram(m_pimkvMKVToolNix->GetMKVInfoPath());
    m_qprocMKVToolNix.setArguments(qstrlArguments);
    m_qprocMKVToolNix.start();
}


void IUIInfoDisplay::MKVToolNixOutputText()
{
    if (m_iPhase == Inactive)
        return;

    if (m_iPhase == MKVMerge)
        MergeProcessOutput();
    else if (m_iPhase == MKVInfo)
        InfoProcessOutput();
}


void IUIInfoDisplay::MergeProcessOutput()
{
    m_pqpteMergeText->insertPlainText(m_qprocMKVToolNix.readAllStandardOutput());
}


void IUIInfoDisplay::MergeFinaliseProcessing()
{
    m_pqpteMergeText->moveCursor(QTextCursor::Start);
}


void IUIInfoDisplay::InfoProcessOutput()
{
    QString qstrMKVInfoOutput;
    if (m_qstrPartialLine.isEmpty() == false)
    {
        qstrMKVInfoOutput = m_qstrPartialLine + QString::fromUtf8(m_qprocMKVToolNix.readAllStandardOutput());
        m_qstrPartialLine.clear();
    }
    else
    {
        qstrMKVInfoOutput = QString::fromUtf8(m_qprocMKVToolNix.readAllStandardOutput());
    }
    m_pqpteInfoText->insertPlainText(qstrMKVInfoOutput);

    int iIndex = 0;
    int iDepth;
    int iDepthDif;
    QString qstrLine;
    QTreeWidgetItem* pqtwiNewItem = nullptr;
    while (InfoGetNextLine(qstrMKVInfoOutput, qstrLine, iIndex, iDepth))
    {
        // For verification only - should be commented out for final release
        // InfoVerifyOutput(qstrLine, iDepth);

        if (iDepth > m_iCurrentDepth)
        {
            m_pqtwiParent = m_pqtwiLastItem;
            m_iCurrentDepth = iDepth;
        }
        else if (iDepth < m_iCurrentDepth)
        {
            if (iDepth == 0)
            {
                m_pqtwiParent = m_pqtwInfoTree->invisibleRootItem();
            }
            else
            {
                iDepthDif = m_iCurrentDepth - iDepth;
                while (--iDepthDif >= 0)
                    m_pqtwiParent = m_pqtwiParent->parent();
            }
            m_iCurrentDepth = iDepth;
        }

        if (m_bScanVerbose)
            m_pdspScanProgress->UpdateProgress(qstrLine, iDepth);

        pqtwiNewItem = new QTreeWidgetItem;
        pqtwiNewItem->setText(0, qstrLine);
        m_pqtwiParent->addChild(pqtwiNewItem);
        m_pqtwiLastItem = pqtwiNewItem;
    }
}


bool IUIInfoDisplay::InfoGetNextLine(const QString & krqstrMKVInfoOutput, QString & rqstrLine, int & riIndex, int & riDepth)
{
    int iStart = riIndex;
    int iOutputLength = krqstrMKVInfoOutput.length();
    while (riIndex < iOutputLength && krqstrMKVInfoOutput.at(riIndex) != '\n')
        ++riIndex;

    if (riIndex >= iOutputLength)
    {
        if (iStart != iOutputLength)
            m_qstrPartialLine = krqstrMKVInfoOutput.mid(iStart);
        return false;
    }

    #ifdef Q_OS_WIN
    // On windows the endline should always be \r\n so the below code should always be:  rqstrLine = krqstrMKVInfoOutput.mid(iStart, (riIndex-iStart)-1);
    // However, MKVToolNix v20 contains a bug where \n was used on Windows rather than \r\n, hence why why need this if statement to check if the \r is present
    if (krqstrMKVInfoOutput.at(riIndex-1) == '\r')
        rqstrLine = krqstrMKVInfoOutput.mid(iStart, (riIndex-iStart)-1);
    else
        rqstrLine = krqstrMKVInfoOutput.mid(iStart, riIndex-iStart);
    #else
    rqstrLine = krqstrMKVInfoOutput.mid(iStart, riIndex-iStart);
    #endif
    ++riIndex;

    int iLnIndex = 0;
    while (rqstrLine.at(iLnIndex) != '|' && rqstrLine.at(iLnIndex) != '+')
        ++iLnIndex;

    riDepth = iLnIndex;
    while (rqstrLine.at(iLnIndex) != '+')
        ++iLnIndex;
    riDepth = iLnIndex - riDepth;

    rqstrLine.remove(0, iLnIndex+2);
    return true;
}


void IUIInfoDisplay::InfoFinaliseProcessing()
{
    m_pqpbSaveInfo->setEnabled(true);
    m_pqpbSaveMerge->setEnabled(true);
    m_pqpbClear->setEnabled(true);
    m_pmwMainWindow->GetMenuBar()->SetSaveEnabled(true);
    m_pqpteInfoText->moveCursor(QTextCursor::Start);

    if (m_bScanVerbose == false)
    {
        m_pqtwInfoTree->expandAll();
    }
    else
    {
        InfoExpandBranchesVerbose(m_pqtwInfoTree->invisibleRootItem());
        m_pdspScanProgress->ScanComplete(m_pdspScanProgress);
    }
}


void IUIInfoDisplay::InfoExpandBranchesVerbose(QTreeWidgetItem* pqtwiItem)
{
    if (pqtwiItem->text(0).startsWith("Seek head") || pqtwiItem->text(0).startsWith("Cluster at") || pqtwiItem->text(0).startsWith("Cues at"))
        return;

    pqtwiItem->setExpanded(true);
    for (int iIndex = 0 ; iIndex < pqtwiItem->childCount(); ++iIndex)
        InfoExpandBranchesVerbose(pqtwiItem->child(iIndex));
}


void IUIInfoDisplay::InfoVerifyOutput(const QString & krqstLine, const int kiDepth)
{
    // This function was added purely for testing and left in if it's necessary to veryify the output again.
    // Note that it only works once and opening another file will lead to the last output file being overwritten.

    static QFile qfilOut;
    static QTextStream qtsrmOut(&qfilOut);
    static bool bOpen = false;
    if (bOpen == false)
    {
        QString qstrOutputFilePath = m_qstrOpenFilePath;
        qstrOutputFilePath.replace(".mkv", "-Verify.txt", Qt::CaseInsensitive);
        qfilOut.setFileName(qstrOutputFilePath);
        qfilOut.open(QIODevice::WriteOnly | QIODevice::Text);
        bOpen = true;
    }

    switch (kiDepth)
    {
    case 0  : qtsrmOut << "+ " << krqstLine << endl;
            break;
    case 1  : qtsrmOut << "|+ " << krqstLine << endl;
            break;
    case 2  : qtsrmOut << "| + " << krqstLine << endl;
            break;
    case 3  : qtsrmOut << "|  + " << krqstLine << endl;
            break;
    case 4  : qtsrmOut << "|   + " << krqstLine << endl;
            break;
    case 5  : qtsrmOut << "|    + " << krqstLine << endl;
            break;
    default : qtsrmOut << "[Possible Depth Issue]\n" << krqstLine << endl;
            break;
    }
}


void IUIInfoDisplay::MKVToolNixErrorText()
{
    QByteArray qbaData = m_qprocMKVToolNix.readAllStandardError();
    QString qstrErrorText = QString::fromUtf8(qbaData);

    QMessageBox::critical(this, tr("Error"), tr("Error while reading track information from the file:\n\n%1\n\n%2")
                          .arg(QDir::toNativeSeparators(m_qstrOpenFilePath))
                          .arg(qstrErrorText), QMessageBox::Ok);
}


void IUIInfoDisplay::MKVToolNixFinished(const int kiExitCode, const QProcess::ExitStatus kqpesExitStatus)
{
    if (m_iPhase == Inactive)
    {
        return;
    }
    else if (kqpesExitStatus == QProcess::CrashExit)
    {
        QMessageBox::critical(this, tr("MKVInfo Crashed"), tr("MKVInfo crashed while reading track information from the file:\n\n%1").arg(QDir::toNativeSeparators(m_qstrOpenFilePath)), QMessageBox::Ok);
        m_iPhase = Inactive;
    }
    else if (kiExitCode == 2)
    {
        QMessageBox::critical(this, tr("MKVInfo Error"), tr("An error occured which prevented reading of track information from the file:\n\n%1").arg(QDir::toNativeSeparators(m_qstrOpenFilePath)), QMessageBox::Ok);
        m_iPhase = Inactive;
    }
    else // Success (iExitCode == 1 actually means warnings but...er...well I'm sure it's fine)
    {
        if (m_iPhase == MKVMerge)
        {
            MergeFinaliseProcessing();
            InitMKVInfo();
        }
        else if (m_iPhase == MKVInfo)
        {
            InfoFinaliseProcessing();
            m_iPhase = Inactive;
        }
    }
}


void IUIInfoDisplay::MKVToolNixError(const QProcess::ProcessError kqpeError)
{
    if (kqpeError == QProcess::FailedToStart)
    {
        QMessageBox::critical(this, tr("MKVInfo Could Not Be Started"), tr("Failed to start MKVInfo.\nFailed to start MKVExtract.\nPlease ensure the MKVToolNix directory has been correctly specified."), QMessageBox::Ok);
    }
}


void IUIInfoDisplay::Abort()
{
    m_iPhase = Inactive;
    m_qprocMKVToolNix.kill();
    m_pdspScanProgress->ScanComplete(m_pdspScanProgress);
    Clear();
}


void IUIInfoDisplay::Clear()
{
    m_pqtwInfoTree->clear();
    m_pqpteInfoText->clear();
    m_pqpteMergeText->clear();

    m_pqpbSaveInfo->setEnabled(false);
    m_pqpbSaveMerge->setEnabled(false);
    m_pqpbClear->setEnabled(false);
    m_pmwMainWindow->GetMenuBar()->SetSaveEnabled(false);

    m_pqleOpenFilePath->clear();
    m_qstrOpenFilePath.clear();
}


void IUIInfoDisplay::SaveMKVInfoOutput()
{
    /*QString qstrOutputFilePath = m_qstrOpenFilePath;
    qstrOutputFilePath.replace(".mkv", "-MKVInfo.txt", Qt::CaseInsensitive);

    QString qstrFilePath = QFileDialog::getSaveFileName(this, tr("Save MKVInfo Output"), qstrOutputFilePath, "*.txt", Q_NULLPTR, QFileDialog::DontResolveSymlinks);
    if (qstrFilePath.isEmpty() == false)
    {
        QFile qfilOut(qstrFilePath);
        qfilOut.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream qtsrmOut(&qfilOut);
        qtsrmOut.setCodec("UTF-8");
        qtsrmOut << m_pqpteInfoText->toPlainText();
        qfilOut.close();
    }*/

    SaveOutput(m_pqpteInfoText, "-MKVInfo.txt", tr("Save MKVInfo Output"));
}


void IUIInfoDisplay::SaveMKVMergeOutput()
{
    SaveOutput(m_pqpteMergeText, "-MKVMergeID.txt", tr("Save MKVMerge Indentify Output"));
}


void IUIInfoDisplay::SaveOutput(QPlainTextEdit* m_pqpteTextEdit, const QString & krqstrFileSuffix, const QString & krqstrSaveDlgTitle)
{
    QString qstrOutputFilePath = m_qstrOpenFilePath;
    qstrOutputFilePath.replace(".mkv", krqstrFileSuffix, Qt::CaseInsensitive);

    QString qstrFilePath = QFileDialog::getSaveFileName(this, krqstrSaveDlgTitle, qstrOutputFilePath, "*.txt", Q_NULLPTR, QFileDialog::DontResolveSymlinks);
    if (qstrFilePath.isEmpty() == false)
    {
        QFile qfilOut(qstrFilePath);
        qfilOut.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream qtsrmOut(&qfilOut);
        qtsrmOut.setCodec("UTF-8");
        qtsrmOut << m_pqpteTextEdit->toPlainText();
        qfilOut.close();
    }
}



void IUIInfoDisplay::SetEnableVerboseScan(const bool kbScanVerbose)
{
    if (m_bScanVerbose != kbScanVerbose)
    {
        m_bScanVerbose = kbScanVerbose;
        m_pmwMainWindow->GetSettings().setValue("MKVToolNix/ScanVerbose", m_bScanVerbose);

        if (m_qstrOpenFilePath.isEmpty() == false)
            OpenFile(m_qstrOpenFilePath);
    }
}

