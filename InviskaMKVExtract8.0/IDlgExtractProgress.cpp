#include <QtWidgets>
#include "IDlgExtractProgress.h"
#include "IMKVExtractProcess.h"
#include "IUIMainWindow.h"
#include "IUIExtract.h"
#ifdef Q_OS_WIN
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#endif


IDlgExtractProgress::IDlgExtractProgress(IUIMainWindow* pmwMainWindow, const int kiNumFiles) : QDialog(pmwMainWindow)
{
    m_pmwMainWindow = pmwMainWindow;
    m_iCompletedFiles = 0;
    m_iStatus = Extracting;

    setWindowTitle(tr("Extraction Progress"));
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);

    QLabel* pqlblFilesProcessed = new QLabel(tr("Files Processed:"));
    m_pqpbFilesProcessed = new QProgressBar(this);
    m_pqpbFilesProcessed->setFormat("%v/%m");
    m_pqpbFilesProcessed->setMaximum(kiNumFiles);
    m_pqpbFilesProcessed->setValue(0);

    QLabel* pqlblFileProgress = new QLabel(tr("File Progress:"));
    m_pqpbFileProgress = new QProgressBar(this);
    m_pqpbFileProgress->setValue(0);

    #ifdef Q_OS_WIN
    QWinTaskbarButton* pqtbbTaskbarButton = new QWinTaskbarButton(this);
    pqtbbTaskbarButton->setWindow(m_pmwMainWindow->windowHandle());
    m_pqtbpTaskbarProgress = pqtbbTaskbarButton->progress();
    m_pqtbpTaskbarProgress->setMaximum(kiNumFiles*100);
    m_pqtbpTaskbarProgress->show();
    #endif

    m_pqlblStatusMessage = new QLabel(tr("<h2>Status: Initialising</h2>"));
    m_pqlblDetailedMessage = new QLabel;
    m_pqlblStatusMessage->setAlignment(Qt::AlignHCenter);
    m_pqlblDetailedMessage->setAlignment(Qt::AlignHCenter);

    m_pqpbAbortClose = new QPushButton("Abort", this);
    m_pqpbAbortClose->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(m_pqpbAbortClose, SIGNAL(clicked()), this, SLOT(AbortCloseClicked()));

    QVBoxLayout* qvblLabels = new QVBoxLayout;
    qvblLabels->addWidget(pqlblFilesProcessed);
    qvblLabels->addWidget(pqlblFileProgress);

    QVBoxLayout* qvblBars = new QVBoxLayout;
    qvblBars->addWidget(m_pqpbFilesProcessed);
    qvblBars->addWidget(m_pqpbFileProgress);

    QHBoxLayout* qhblProgressLayout = new QHBoxLayout;
    qhblProgressLayout->addLayout(qvblLabels);
    qhblProgressLayout->addLayout(qvblBars);

    QVBoxLayout* qvblLayout = new QVBoxLayout;
    qvblLayout->addLayout(qhblProgressLayout);
    qvblLayout->addWidget(m_pqlblStatusMessage);
    qvblLayout->addWidget(m_pqlblDetailedMessage);
    qvblLayout->addStretch();
    qvblLayout->addWidget(m_pqpbAbortClose, 0, Qt::AlignCenter);

    setLayout(qvblLayout);
    resize(400, sizeHint().height());
    show();
}


void IDlgExtractProgress::UpdateFileProgress(const int kiPercentage)
{
    m_pqpbFileProgress->setValue(kiPercentage);

    #ifdef Q_OS_WIN
    int iNewValue = m_iCompletedFiles*100 + kiPercentage;
    if (iNewValue > m_pqtbpTaskbarProgress->value())
        m_pqtbpTaskbarProgress->setValue(iNewValue);
    #endif
}


void IDlgExtractProgress::IncrementFilesProcessed()
{
    m_pqpbFilesProcessed->setValue(++m_iCompletedFiles);
}


void IDlgExtractProgress::SetFileBeingProcessed(const QString & krqstrFileName)
{
    m_pqlblDetailedMessage->setText(krqstrFileName);
}


void IDlgExtractProgress::SetPhase(const int kiExtractionPhase)
{
    switch (kiExtractionPhase)
    {
    case IMKVExtractProcess::Initial      : m_pqlblStatusMessage->setText(tr("<h2>Status: Initialising</h2>"));
                                            break;
    case IMKVExtractProcess::Tracks       : m_pqlblStatusMessage->setText(tr("<h2>Status: Extracting Tracks</h2>"));
                                            break;
    case IMKVExtractProcess::Timestamps   : m_pqlblStatusMessage->setText(tr("<h2>Status: Extracting Timestamps</h2>"));
                                            break;
    case IMKVExtractProcess::Cues         : m_pqlblStatusMessage->setText(tr("<h2>Status: Extracting Cues</h2>"));
                                            break;
    case IMKVExtractProcess::Chapters     : m_pqlblStatusMessage->setText(tr("<h2>Status: Extracting Chapters</h2>"));
                                            break;
    case IMKVExtractProcess::Cuesheet     : m_pqlblStatusMessage->setText(tr("<h2>Status: Extracting Cuesheet</h2>"));
                                            break;
    case IMKVExtractProcess::Tags         : m_pqlblStatusMessage->setText(tr("<h2>Status: Extracting Tags</h2>"));
                                            break;
    case IMKVExtractProcess::Attachmenets : m_pqlblStatusMessage->setText(tr("<h2>Status: Extracting Attachments</h2>"));
                                            break;
    case IMKVExtractProcess::Complete     : m_pqlblStatusMessage->setText(tr("<h2>Status: Extracton Complete</h2>"));
                                            break;
    case IMKVExtractProcess::Aborted      : m_pqlblStatusMessage->setText(tr("<h2>Status: Extraction Aborted</h2>"));
                                            break;
    }
}


void IDlgExtractProgress::SetComplete()
{
    m_pqlblStatusMessage->setText(tr("<h2>Status: Extracton Complete</h2>"));
    m_pqlblDetailedMessage->setText(tr("Extraction completed successfully."));
    m_pqpbAbortClose->setText(tr("Close"));
    m_iStatus = Completed;
    m_pqpbFileProgress->setValue(100);

    #ifdef Q_OS_WIN
    m_pqtbpTaskbarProgress->hide();
    #endif
}


void IDlgExtractProgress::SetAborted()
{
    m_pqlblStatusMessage->setText(tr("<h2>Status: Extracton Aborted</h2>"));
    m_pqlblDetailedMessage->setText(tr("Partially extracted tracks will be incomplete and unusable."));
    m_pqpbAbortClose->setText(tr("Close"));
    m_iStatus = Aborted;

    #ifdef Q_OS_WIN
    m_pqtbpTaskbarProgress->hide();
    #endif
}


void IDlgExtractProgress::AbortCloseClicked()
{
    if (m_iStatus == Extracting)
    {
        m_pmwMainWindow->GetExtractUI()->AbortCurrentExtraction();
        SetAborted();
    }
    else
    {
        accept();
    }
}
