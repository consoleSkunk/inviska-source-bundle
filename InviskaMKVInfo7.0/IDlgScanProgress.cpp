#include <QtWidgets>
#include "IDlgScanProgress.h"
#include "IUIMainWindow.h"
#include "IUIInfoDisplay.h"
#ifdef Q_OS_WIN
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#endif


IDlgScanProgress::IDlgScanProgress(IUIMainWindow* pmwMainWindow, IUIInfoDisplay* puiidInfoDisplay, const QString & krqstrFilePath) : QDialog(pmwMainWindow)
{
    m_iFileSize = 0;
    m_iUpdateIntervalCount = 0;

    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);

    m_pqpbScanProgress = new QProgressBar(this);

    #ifdef Q_OS_WIN
    QWinTaskbarButton* pqtbbTaskbarButton = new QWinTaskbarButton(this);
    pqtbbTaskbarButton->setWindow(pmwMainWindow->windowHandle());
    m_pqtbpTaskbarProgress = pqtbbTaskbarButton->progress();
    #endif

    m_pqlblStatusMessage = new QLabel(tr("<h2>Scan In Progress</h2>Scanning file: %1").arg(krqstrFilePath.mid(krqstrFilePath.lastIndexOf('/')+1)), this);
    m_pqlblStatusMessage->setAlignment(Qt::AlignHCenter);

    m_pqpbAbort = new QPushButton("Abort", this);
    m_pqpbAbort->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(m_pqpbAbort, SIGNAL(clicked()), puiidInfoDisplay, SLOT(Abort()));

    QVBoxLayout* qvblLayout = new QVBoxLayout;
    qvblLayout->addWidget(m_pqlblStatusMessage);
    qvblLayout->addWidget(m_pqpbScanProgress);
    qvblLayout->addStretch();
    qvblLayout->addWidget(m_pqpbAbort);
    qvblLayout->setAlignment(m_pqpbAbort, Qt::AlignCenter);

    resize(sizeHint());
    setLayout(qvblLayout);
}


void IDlgScanProgress::UpdateProgress(const QString & krqstrLine, const int kiDepth)
{
    if (m_iFileSize != 0)
    {
        if (kiDepth < 3)
        {
            if (++m_iUpdateIntervalCount == m_kiProgressUpdateInterval)
            {
                m_iUpdateIntervalCount = 0;
                int iStart = krqstrLine.lastIndexOf(' ');

                // To avoid an integer overflow the last three digits are stripped off all file positions
                int iLength = krqstrLine.length() - iStart - 3;
                if (iLength > 0)
                {
                    int iCurVal = krqstrLine.mid(iStart, iLength).toInt();
                    m_pqpbScanProgress->setValue(iCurVal);

                    #ifdef Q_OS_WIN
                    m_pqtbpTaskbarProgress->setValue(iCurVal);
                    #endif
                }
            }
        }
    }
    else
    {
        if (kiDepth == 0 && krqstrLine.startsWith("Segment"))
        {
            m_iFileSize = krqstrLine.mid(14, krqstrLine.lastIndexOf('a')-18).toInt();
            m_pqpbScanProgress->setMaximum(m_iFileSize);
            m_pqpbScanProgress->setValue(0);
            resize(400, height());
            show();

            #ifdef Q_OS_WIN
            m_pqtbpTaskbarProgress->setMaximum(m_iFileSize);
            m_pqtbpTaskbarProgress->show();
            #endif
        }
    }
}


void IDlgScanProgress::ScanComplete(IDlgScanProgress* & rpdspScanProgress)
{
    #ifdef Q_OS_WIN
    m_pqtbpTaskbarProgress->hide();
    #endif

    rpdspScanProgress = nullptr;
    close();
}

