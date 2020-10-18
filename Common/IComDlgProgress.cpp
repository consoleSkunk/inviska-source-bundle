#include <QtWidgets>
#include "IComDlgProgress.h"
#include "IUIMainWindow.h"
#ifdef Q_OS_WIN
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#endif


IComDlgProgress::IComDlgProgress(IUIMainWindow* pmwMainWindow, const QString & krqstrTitle, const QString & krqstrMessage, const int kiMax, const bool kbDisplayPercent, const int kiAppearDelayMS) : QDialog(pmwMainWindow)
{
    m_bAborted = false;

    setWindowTitle(krqstrTitle);
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);

    QLabel* pqlblActionMessage = new QLabel("<h2>" + krqstrTitle + "</h2>");
    m_pqlblMessage = new QLabel(krqstrMessage);
    pqlblActionMessage->setAlignment(Qt::AlignHCenter);
    m_pqlblMessage->setAlignment(Qt::AlignHCenter);

    m_pqpbProgress = new QProgressBar(this);
    m_pqpbProgress->setMaximum(kiMax);
    m_pqpbProgress->setValue(0);
    if (kbDisplayPercent == false)
        m_pqpbProgress->setFormat("%v/%m");

    #ifdef Q_OS_WIN
    QWinTaskbarButton* pqtbbTaskbarButton = new QWinTaskbarButton(this);
    pqtbbTaskbarButton->setWindow(pmwMainWindow->windowHandle());
    m_pqtbpTaskbarProgress = pqtbbTaskbarButton->progress();
    m_pqtbpTaskbarProgress->setMaximum(kiMax*100);
    m_pqtbpTaskbarProgress->show();
    #endif

    m_pqpbAbort = new QPushButton("Abort", this);
    m_pqpbAbort->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(m_pqpbAbort, SIGNAL(clicked()), this, SLOT(AbortClicked()));

    QVBoxLayout* qvblLayout = new QVBoxLayout;
    qvblLayout->addWidget(m_pqpbProgress);
    qvblLayout->addWidget(pqlblActionMessage);
    qvblLayout->addWidget(m_pqlblMessage);
    qvblLayout->addStretch();
    qvblLayout->addWidget(m_pqpbAbort);
    qvblLayout->setAlignment(m_pqpbAbort, Qt::AlignCenter);

    setLayout(qvblLayout);
    resize(400, sizeHint().height());

    QTimer::singleShot(kiAppearDelayMS, this, SLOT(show()));
}


IComDlgProgress::~IComDlgProgress()
{
    #ifdef Q_OS_WIN
    m_pqtbpTaskbarProgress->hide();
    #endif
}


void IComDlgProgress::UpdateMessage(const QString & krqstrMessage)
{
    m_pqlblMessage->setText(krqstrMessage);
}


void IComDlgProgress::UpdateProgress(const int kiCurrentVal)
{
    m_pqpbProgress->setValue(kiCurrentVal);

    #ifdef Q_OS_WIN
    int iNewValue = kiCurrentVal*100;
    if (iNewValue > m_pqtbpTaskbarProgress->value())
        m_pqtbpTaskbarProgress->setValue(iNewValue);
    #endif

    QApplication::processEvents();
}


void IComDlgProgress::IncrementAndUpdateProgress()
{
    UpdateProgress(m_pqpbProgress->value()+1);
}


void IComDlgProgress::AbortClicked()
{
    m_bAborted = true;
    m_pqpbAbort->setEnabled(false);

    #ifdef Q_OS_WIN
    m_pqtbpTaskbarProgress->hide();
    #endif
}
