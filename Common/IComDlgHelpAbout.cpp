#include <QtWidgets>
#include "IComDlgHelpAbout.h"
#include "IUIMainWindow.h"


IComDlgHelpAbout::IComDlgHelpAbout(IUIMainWindow* pmwMainWindow) : QDialog(pmwMainWindow)
{
    m_pmwMainWindow = pmwMainWindow;

    setWindowTitle(tr("About %1").arg(m_pmwMainWindow->GetAppName()));
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QPixmap qpixmTitleImage(":/Resources/HelpAboutBanner.png");
    QLabel* pqlblTitleBanner = new QLabel;
    pqlblTitleBanner->setPixmap(qpixmTitleImage);
    pqlblTitleBanner->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    QString qstrInfo = tr("<center><h2>%1</h2>"
                          "<p>Current version: %2<br>Latest version: %3</p>"
                          "<a href='%4'>%4</a></p>"
                          "<p>%5 is released under the GPLv2+ licence.</p>"
                          "<p>It is provided as is with no warranty of any kind, including the <br>warranty of design, merchantability and fitness for a particular purpose.</p>"
                          "<p>If you require assistance, or wish to report a bug, please visit the <a href='%6'>forum</a>.</p>"
                          "<br></center>")
                          .arg(m_pmwMainWindow->GetAppName())
                          .arg(m_pmwMainWindow->GetVersion())
                          .arg(m_pmwMainWindow->GetVersionInfo()->GetLatestVersion())
                          .arg(m_pmwMainWindow->GetWebsite())
                          .arg(m_pmwMainWindow->GetAppName())
                          .arg(m_pmwMainWindow->GetForumAddress());
    QLabel* pqlblInfo = new QLabel(qstrInfo);
    pqlblInfo->setTextFormat(Qt::RichText);
    //pqlblInfo->setTextInteractionFlags(Qt::TextBrowserInteraction);
    pqlblInfo->setOpenExternalLinks(true);

    QLabel* pqlblMsg = nullptr;
    IComSysLatestVersion* pilvMsg = m_pmwMainWindow->GetVersionInfo();
    if (pilvMsg->MessageAvailable())
    {
        pqlblMsg = new QLabel("<center><h1>" + pilvMsg->GetMessageTitle() + "</h1>" + pilvMsg->GetMessageBody() + "<p></p></center>");
        pqlblMsg->setFrameStyle(QFrame::Panel | QFrame::Sunken);
        pqlblMsg->setOpenExternalLinks(true);
    }

    m_pqdbbOK = new QDialogButtonBox(QDialogButtonBox::Ok);
    m_pqdbbOK->setOrientation(Qt::Horizontal);
    connect(m_pqdbbOK, SIGNAL(accepted()), this, SLOT(accept()));

    m_pqpbAboutQt = new QPushButton(tr("About Qt"));
    connect(m_pqpbAboutQt, SIGNAL(clicked()), this, SLOT(ShowAboutQt()));

    QHBoxLayout* phblButtonLayout = new QHBoxLayout;
    phblButtonLayout->addWidget(m_pqpbAboutQt);
    phblButtonLayout->addWidget(m_pqdbbOK);

    QVBoxLayout* pqvblLayout = new QVBoxLayout;
    pqvblLayout->addWidget(pqlblTitleBanner);
    pqvblLayout->addWidget(pqlblInfo);
    if (pqlblMsg != nullptr)
        pqvblLayout->addWidget(pqlblMsg);
    pqvblLayout->addStretch();
    pqvblLayout->addLayout(phblButtonLayout);

    setLayout(pqvblLayout);
    resize(sizeHint());
    show();
}


void IComDlgHelpAbout::ShowAboutQt()
{
    QMessageBox::aboutQt(this);
}
