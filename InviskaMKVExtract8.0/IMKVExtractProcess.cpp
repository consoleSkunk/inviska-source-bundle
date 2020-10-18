#include <QtWidgets>
#include "IMKVExtractProcess.h"
#include "IMKVCodecLookup.h"
#include "IUIExtract.h"
#include "IDlgExtractProgress.h"
#include "IComSysMKVToolNix.h"


IMKVExtractProcess::IMKVExtractProcess(IUIExtract* puiextExtractUI, QTreeWidgetItem* pqtwiFileInfo)// : QObject(puiextExtractUI)
{
    m_pqtwiFileInfo         = pqtwiFileInfo;
    m_puiextExtractUI       = puiextExtractUI;
    m_pdepProgressDlg       = puiextExtractUI->GetProgressDialog();
    m_iTracksEndIndex       = -1;
    m_iChaptersIndex        = -1;
    m_iCuesheetIndex        = -1;
    m_iTagsIndex            = -1;
    m_iAttachmentsIndex     = -1;
    m_iPhase                = Initial;

    m_qprocMKVExtract.setProgram(m_puiextExtractUI->GetMKVToolNix()->GetMKVExtractPath());
    connect(&m_qprocMKVExtract, SIGNAL(readyReadStandardOutput()),             this, SLOT(MKVExtractOutputText()));
    connect(&m_qprocMKVExtract, SIGNAL(readyReadStandardError()),              this, SLOT(MKVExtractErrorText()));
    connect(&m_qprocMKVExtract, SIGNAL(finished(int, QProcess::ExitStatus)),   this, SLOT(MKVExtractFinished(int, QProcess::ExitStatus)));
    connect(&m_qprocMKVExtract, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(MKVExtractError(QProcess::ProcessError)));

    #ifdef Q_OS_MACOS
    m_qprocMKVExtract.setEnvironment(QStringList("LANG=en_US.UTF-8"));
    #endif

    SetOutputPath();
    SetItemIndecies();
}


void IMKVExtractProcess::BeginExtract()
{
    PerformNextPhase();
}


void IMKVExtractProcess::SetOutputPath()
{
    m_qstrOutputPath = m_puiextExtractUI->GetOuptputPath();
    if (m_qstrOutputPath.isEmpty())
    {
        m_qstrOutputPath = m_pqtwiFileInfo->data(0, IUIExtract::FilePathRoll).toString();
        m_qstrOutputPath = m_qstrOutputPath.left(m_qstrOutputPath.lastIndexOf(QDir::separator())+1);
    }
    else
    {
        if (m_qstrOutputPath.at(m_qstrOutputPath.length()-1) != QDir::separator())
            m_qstrOutputPath.append(QDir::separator());
    }
}


void IMKVExtractProcess::SetItemIndecies()
{
    int iPhase;
    int iNumChildren = m_pqtwiFileInfo->childCount();
    for (int iIndex = 0 ; iIndex < iNumChildren ; ++iIndex)
    {
        iPhase = GetItemPhase(m_pqtwiFileInfo->child(iIndex)->type());
        switch(iPhase)
        {
        case Tracks       : m_iTracksEndIndex = iIndex;
                            break;

        case Chapters     : m_iChaptersIndex = iIndex;
                            break;

        case Cuesheet     : m_iCuesheetIndex = iIndex;
                            break;

        case Tags         : m_iTagsIndex = iIndex;
                            break;

        case Attachmenets : m_iAttachmentsIndex = iIndex;
                            break;
        }
    }
}


void IMKVExtractProcess::PerformNextPhase()
{
    switch(++m_iPhase)
    {
    case Tracks       :
    case Timestamps   :
    case Cues         : ExtractTracksTimestampsCues();
                        break;

    case Chapters     :
    case Cuesheet     :
    case Tags         : ExtractChaptersCuesheetTags();
                        break;

    case Attachmenets : ExtractAttachments();
                        break;

    case Complete     : emit ExtractionComplete(m_pqtwiFileInfo);
                        break;
    }
}


void IMKVExtractProcess::ExtractTracksTimestampsCues()
{
    if (m_iTracksEndIndex == -1 || (m_iPhase == Timestamps && m_puiextExtractUI->ExtractTimestamps() == false) || (m_iPhase == Cues && m_puiextExtractUI->ExtractCues() == false))
    {
        PerformNextPhase();
        return;
    }
    m_pdepProgressDlg->SetPhase(m_iPhase);

    m_qstrlArguments.clear();
    if (m_puiextExtractUI->GetMKVToolNix()->NewMKVExtractInterface())
    {
        // mkvextract source-filename tracks [options] TID1:dest-filename1 [TID2:dest-filename2 ...]
        // mkvextract source-filename timestamps_v2 [options] TID1:dest-filename1 [TID2:dest-filename2 ...]
        // mkvextract source-filename cues [options] TID1:dest-filename1 [TID2:dest-filename2 ...]
        m_qstrlArguments.append(m_pqtwiFileInfo->data(0, IUIExtract::FilePathRoll).toString());
        m_qstrlArguments.append(m_iPhase == Tracks ? "tracks" : m_iPhase == Timestamps ? "timestamps_v2" : "cues");
        m_qstrlArguments.append("--gui-mode");
        //qDebug() << "Extracting tracks with New interface";
    }
    else
    {
        // mkvextract tracks       {source-filename} [options] TID1:dest-filename1 [TID2:dest-filename2 ...]
        // mkvextract timecodes_v2 {source-filename} [options] TID1:dest-filename1 [TID2:dest-filename2 ...]
        // mkvextract cues         {source-filename} [options] TID1:dest-filename1 [TID2:dest-filename2 ...]
        m_qstrlArguments.append(m_iPhase == Tracks ? "tracks" : m_iPhase == Timestamps ? "timecodes_v2" : "cues");
        m_qstrlArguments.append(m_pqtwiFileInfo->data(0, IUIExtract::FilePathRoll).toString());
        m_qstrlArguments.append("--gui-mode");
        //qDebug() << "Extracting tracks with Old interface";
    }

    QTreeWidgetItem* pqtwiCurrentTrack;
    int iTrackType = IUIExtract::Invalid;
    int iCurrentTrackType = IUIExtract::Invalid;
    int iCurrentTrackInstance = 1;
    bool bNothingToExtract = true;

    for (int iIndex = 0 ; iIndex <= m_iTracksEndIndex ; ++iIndex)
    {
        pqtwiCurrentTrack = m_pqtwiFileInfo->child(iIndex);
        iTrackType = pqtwiCurrentTrack->type();
        if (iTrackType != iCurrentTrackType)
        {
            iCurrentTrackType = iTrackType;
            iCurrentTrackInstance = 1;
        }
        else
        {
            ++iCurrentTrackInstance;
        }

        if ((pqtwiCurrentTrack->checkState(0) == Qt::Checked || OnBatchExtractList(iCurrentTrackType, iCurrentTrackInstance))
            && pqtwiCurrentTrack->data(0, IUIExtract::CodecSupportedRole).toBool() == true)
        {
            int iTrackID = pqtwiCurrentTrack->data(0, IUIExtract::TrackIDRole).toInt();
            m_qstrlArguments.append(QString::number(iTrackID) + ':' + GenerateFileName(pqtwiCurrentTrack, iCurrentTrackType, iCurrentTrackInstance));
            bNothingToExtract = false;
        }
    }

    if (bNothingToExtract)
    {
        PerformNextPhase();
    }
    else
    {
        m_qprocMKVExtract.setArguments(m_qstrlArguments);
        m_qprocMKVExtract.start();
    }
}


void IMKVExtractProcess::ExtractChaptersCuesheetTags()
{
    if (m_iPhase == Chapters)
    {
        if (m_iChaptersIndex == -1 || (m_pqtwiFileInfo->child(m_iChaptersIndex)->checkState(0) == Qt::Unchecked && OnBatchExtractList(IUIExtract::Chapters, 0) == false))
        {
            PerformNextPhase();
            return;
        }
    }
    else if (m_iPhase == Cuesheet)
    {
        if (m_iCuesheetIndex == -1 || (m_pqtwiFileInfo->child(m_iCuesheetIndex)->checkState(0) == Qt::Unchecked && OnBatchExtractList(IUIExtract::Cuesheet, 0) == false))
        {
            PerformNextPhase();
            return;
        }
    }
    else if (m_iPhase == Tags)
    {
        if (m_iTagsIndex == -1 || (m_pqtwiFileInfo->child(m_iTagsIndex)->checkState(0) == Qt::Unchecked && OnBatchExtractList(IUIExtract::Tags, 0) == false))
        {
            PerformNextPhase();
            return;
        }
    }
    m_pdepProgressDlg->SetPhase(m_iPhase);


    QString qstrSuffix = m_iPhase == Chapters ? "_Chapters.xml" : m_iPhase == Cuesheet ? "_Cuesheet.cue" : "_Tags.xml";
    QString qstrFileName = m_pqtwiFileInfo->data(0, IUIExtract::FilePathRoll).toString();
    qstrFileName = qstrFileName.mid(qstrFileName.lastIndexOf(QDir::separator())+1);
    qstrFileName = qstrFileName.left(qstrFileName.lastIndexOf('.'));
    qstrFileName = m_qstrOutputPath + qstrFileName + qstrSuffix;

    m_qstrlArguments.clear();
    if (m_puiextExtractUI->GetMKVToolNix()->NewMKVExtractInterface())
    {
        // mkvextract source-filename chapters [options] output-filename.xml
        // mkvextract source-filename cuesheet [options] output-filename.cue
        // mkvextract source-filename tags [options] output-filename.xml
        m_qstrlArguments.append(m_pqtwiFileInfo->data(0, IUIExtract::FilePathRoll).toString());
        m_qstrlArguments.append(m_iPhase == Chapters ? "chapters" : m_iPhase == Cuesheet ? "cuesheet" : "tags");
        m_qstrlArguments.append("--gui-mode");
        m_qstrlArguments.append(qstrFileName);
    }
    else
    {
        // mkvextract chapters source-filename [options]
        // mkvextract cuesheet source-filename [options]
        // mkvextract tags     source-filename [options]
        m_qstrlArguments.append(m_iPhase == Chapters ? "chapters" : m_iPhase == Cuesheet ? "cuesheet" : "tags");
        m_qstrlArguments.append(m_pqtwiFileInfo->data(0, IUIExtract::FilePathRoll).toString());
        m_qstrlArguments.append("--gui-mode");
        m_qstrlArguments.append("--redirect-output");
        m_qstrlArguments.append(qstrFileName);
    }

    m_qprocMKVExtract.setArguments(m_qstrlArguments);
    m_qprocMKVExtract.start();
}


void IMKVExtractProcess::ExtractAttachments()
{
    bool bBatchExtractAttachmenets = OnBatchExtractList(IUIExtract::AttachmentsGroup, 0);
    if (m_iAttachmentsIndex == -1 || (m_pqtwiFileInfo->child(m_iAttachmentsIndex)->checkState(0) == Qt::Unchecked && bBatchExtractAttachmenets == false))
    {
        PerformNextPhase();
        return;
    }
    m_pdepProgressDlg->SetPhase(m_iPhase);

    QString qstrFileName = m_pqtwiFileInfo->data(0, IUIExtract::FilePathRoll).toString();
    qstrFileName = qstrFileName.mid(qstrFileName.lastIndexOf(QDir::separator())+1);
    qstrFileName = qstrFileName.left(qstrFileName.lastIndexOf('.'));

    QDir qdirDir;
    QString qstrAttOutputPath = m_qstrOutputPath + qstrFileName + tr("_Attachments", "Directory attachments are extracted to");
    if (QFileInfo::exists(qstrAttOutputPath) == false)
    {
        if (qdirDir.mkdir(qstrAttOutputPath) == false)
        {
            QMessageBox::critical(m_puiextExtractUI, tr("Error Extracting Attachments"),
                                  tr("Error extracting attachments from the file:\n\n%1\n\nCould not create folder:\n\n%2\n\nSkipping attachment extraction.")
                                  .arg(m_pqtwiFileInfo->data(0, IUIExtract::FilePathRoll).toString())
                                  .arg(qstrAttOutputPath), QMessageBox::Ok);

            PerformNextPhase();
            return;
        }
    }
    qstrAttOutputPath.append(QDir::separator());

    m_qstrlArguments.clear();
    if (m_puiextExtractUI->GetMKVToolNix()->NewMKVExtractInterface())
    {
        // mkvextract source-filename attachments [options] AID1:outname1 [AID2:outname2 ...]
        m_qstrlArguments.append(m_pqtwiFileInfo->data(0, IUIExtract::FilePathRoll).toString());
        m_qstrlArguments.append("attachments");
        m_qstrlArguments.append("--gui-mode");
    }
    else
    {
        // mkvextract attachments source-filename [options] AID1:outname1 [AID2:outname2 ...]
        m_qstrlArguments.append("attachments");
        m_qstrlArguments.append(m_pqtwiFileInfo->data(0, IUIExtract::FilePathRoll).toString());
        m_qstrlArguments.append("--gui-mode");
    }

    QTreeWidgetItem* pqtwiAttachmentsGroup = m_pqtwiFileInfo->child(m_iAttachmentsIndex);
    QTreeWidgetItem* pqtwiAttachmentItem = nullptr;
    int iNumAttachmenets = pqtwiAttachmentsGroup->childCount();
    for (int iIndex = 0 ; iIndex < iNumAttachmenets ; ++iIndex)
    {
        pqtwiAttachmentItem = pqtwiAttachmentsGroup->child(iIndex);
        if (pqtwiAttachmentItem->checkState(0) == Qt::Checked || bBatchExtractAttachmenets)
            m_qstrlArguments.append(pqtwiAttachmentItem->data(0, IUIExtract::TrackIDRole).toString() + ':' + qstrAttOutputPath + pqtwiAttachmentItem->text(0));
    }


    m_qprocMKVExtract.setArguments(m_qstrlArguments);
    m_qprocMKVExtract.start();
}


QString IMKVExtractProcess::GenerateFileName(QTreeWidgetItem* pqtwiTrackInfo, const int kiTrackType, const int kiTrackInstance)
{
    QString qstrFileName = m_pqtwiFileInfo->data(0, IUIExtract::FilePathRoll).toString();
    QString qstrCodecID = pqtwiTrackInfo->data(0, IUIExtract::CodecIDRole).toString();
    qstrFileName = qstrFileName.mid(qstrFileName.lastIndexOf(QDir::separator())+1);
    qstrFileName = qstrFileName.left(qstrFileName.lastIndexOf('.'));

    QString qstrTrackSuffix;
    QString qstrTrackInstance = QString::asprintf("%02d", kiTrackInstance);
    switch (kiTrackType)
    {
    case IUIExtract::Video      :   qstrTrackSuffix = tr("_Video%1").arg(qstrTrackInstance);
                                    break;
    case IUIExtract::Audio      :   qstrTrackSuffix = tr("_Audio%1").arg(qstrTrackInstance);
                                    break;
    case IUIExtract::Subtitles  :   qstrTrackSuffix = tr("_Subtitles%1").arg(qstrTrackInstance);
                                    break;
    case IUIExtract::Chapters   :   qstrTrackSuffix = tr("_Chapters");
                                    break;
    }

    if (m_iPhase == Tracks)
        return m_qstrOutputPath + qstrFileName + qstrTrackSuffix + '.' + IMKVCodecLookup::LookupExtension(qstrCodecID);
    else if (m_iPhase == Timestamps)
        return m_qstrOutputPath + qstrFileName + qstrTrackSuffix + "-Timestamps.txt";
    else if (m_iPhase == Cues)
        return m_qstrOutputPath + qstrFileName + qstrTrackSuffix + "-Cues.txt";

    // This shouldn't happen
    return m_qstrOutputPath + qstrFileName + "-Error";
}


bool IMKVExtractProcess::OnBatchExtractList(const int kiTrackType, const int kiTrackInstance)
{
    QListWidget* pqlwBatchExtractList = m_puiextExtractUI->GetBatchExtractList();

    int iNumItems = pqlwBatchExtractList->count();
    for (int iRow = 0 ; iRow < iNumItems ; ++iRow)
    {
        if (pqlwBatchExtractList->item(iRow)->type() == kiTrackType || pqlwBatchExtractList->item(iRow)->type() == kiTrackType+kiTrackInstance)
        {
            if (pqlwBatchExtractList->item(iRow)->checkState() == Qt::Checked)
                return true;
            return false;
        }
    }
    return false;
}


int IMKVExtractProcess::GetItemPhase(const int kiItemType)
{
    switch (kiItemType)
    {
    case IUIExtract::Video              :
    case IUIExtract::Audio              :
    case IUIExtract::Subtitles          : return Tracks;

    case IUIExtract::Chapters           : return Chapters;

    case IUIExtract::Cuesheet           : return Cuesheet;

    case IUIExtract::Tags               : return Tags;

    case IUIExtract::AttachmentsGroup   : return Attachmenets;
    }

    return Invalid;
}


void IMKVExtractProcess::MKVExtractOutputText()
{
    QByteArray qbaData = m_qprocMKVExtract.readAllStandardOutput();
    QString qstrMKVExtractOutput = QString::fromLocal8Bit(qbaData);

    // On Windows progress messages are in the form "#GUI#progress ".
    // On Linux they're in the form "Progress: " even though the --gui-mode swich is used.
    // So it works with both we'll just find the last % and read the number before it.

    int iPercentIndex = qstrMKVExtractOutput.lastIndexOf('%');
    if (iPercentIndex > 4)
    {
        if (qstrMKVExtractOutput.at(iPercentIndex-3) == ' ')
            m_pdepProgressDlg->UpdateFileProgress(qstrMKVExtractOutput.mid(iPercentIndex-2, 2).toInt());
        else if (qstrMKVExtractOutput.at(iPercentIndex-2) == ' ')
            m_pdepProgressDlg->UpdateFileProgress(qstrMKVExtractOutput.mid(iPercentIndex-1, 1).toInt());
        else if (qstrMKVExtractOutput.at(iPercentIndex-4) == ' ')
            m_pdepProgressDlg->UpdateFileProgress(qstrMKVExtractOutput.mid(iPercentIndex-3, 3).toInt());
    }
}


void IMKVExtractProcess::MKVExtractErrorText()
{
    QByteArray qbaData = m_qprocMKVExtract.readAllStandardError();
    QString qstrErrorText = QString::fromLocal8Bit(qbaData);

    QMessageBox::critical(m_puiextExtractUI, tr("Error"), tr("Error while extracting tracks from the file:\n\n%1\n\n%2")
                          .arg(m_pqtwiFileInfo->data(0, IUIExtract::FilePathRoll).toString())
                          .arg(qstrErrorText), QMessageBox::Ok);
}


void IMKVExtractProcess::MKVExtractFinished(const int kiExitCode, const QProcess::ExitStatus kqpesExitStatus)
{
    if (m_iPhase == Aborted)
    {
        return;
    }
    else if (kqpesExitStatus == QProcess::CrashExit)
    {
        QMessageBox::critical(m_puiextExtractUI, tr("MKVExtract Crashed"), tr("MKVExtract crashed while extracting tracks from the file:\n\n%1").arg(m_pqtwiFileInfo->data(0, IUIExtract::FilePathRoll).toString()), QMessageBox::Ok);
    }
    else if (kiExitCode == 2 && m_iPhase != Cues)
    {
        // MKVExtract will exit with code 2 if you request extraction of cues for a track where cues are not present.
        // Generally, cues are only present for video tracks, and not audio or subtitle tracks, so this condition occurs quite often.
        // We therefore don't trigger this error when extracting cues, so the else condition is executed and we continue to the next phase.
        // A more precise solution would be to check for the below error in MKVExtractOutputText() when m_iPhase == Cues, and if present set a bNoCuesPresent flag.
        //          #GUI#error There are no cues for track ID 0.
        // However, we can be pretty much certain that if we're extracting cues and the exit code is 2 then it's because there are no cues present for the track, and doing it this way keps the code a bit tider.
        // It would better if we could check for the presence of cues from the MKVInfo output, but I can't see any way of doing this wihtout using verbose mode, which would be way too slow.

        QMessageBox::critical(m_puiextExtractUI, tr("MKVExtract Error"), tr("An error occured which prevented extraction of tracks from the file:\n\n%1").arg(m_pqtwiFileInfo->data(0, IUIExtract::FilePathRoll).toString()), QMessageBox::Ok);
    }
    else // Success (kiExitCode == 1 actually means warnings but...er...well I'm sure it's fine)
    {
        m_pdepProgressDlg->UpdateFileProgress(100);
        PerformNextPhase();
    }
}


void IMKVExtractProcess::MKVExtractError(const QProcess::ProcessError kqpeError)
{
    if (kqpeError == QProcess::FailedToStart)
    {
        QMessageBox::critical(m_puiextExtractUI, tr("MKVExtract Could Not Be Started"), tr("Failed to start MKVExtract.\nPlease ensure the MKVToolNix directory has been correctly specified."), QMessageBox::Ok);
    }
}


void IMKVExtractProcess::AbortExtraction()
{
    m_qprocMKVExtract.kill();
    m_iPhase = Aborted;
}
