#include <QtWidgets>
#include "IComDlgCredits.h"
#include "IUIMainWindow.h"
#include "IUIMenuBar.h"


IComDlgCredits::IComDlgCredits(IUIMainWindow* pmwMainWindow) : QDialog(pmwMainWindow)
{
    m_pmwMainWindow = pmwMainWindow;

    setWindowTitle(tr("Credits"));
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QPixmap qpixmTitleImage(":/Resources/HelpAboutBanner.png");
    QLabel* pqlblTitleBanner = new QLabel;
    pqlblTitleBanner->setPixmap(qpixmTitleImage);
    pqlblTitleBanner->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    QVBoxLayout* pqvblLayout = new QVBoxLayout;
    pqvblLayout->addWidget(pqlblTitleBanner);

    AddLabel(pqvblLayout, tr("Translations"), pmwMainWindow->GetMenuBar()->GetCreditsTranslations());
    AddLabel(pqvblLayout, tr("Code Contributions"), pmwMainWindow->GetMenuBar()->GetCreditsCodeContrib());
    AddLabel(pqvblLayout, tr("Dependencies"), pmwMainWindow->GetMenuBar()->GetCreditsDependency());

    m_pqpbOK = new QPushButton(tr("OK"));
    m_pqpbOK->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(m_pqpbOK, SIGNAL(clicked()), this, SLOT(accept()));

    pqvblLayout->addStretch();
    pqvblLayout->addWidget(m_pqpbOK, 0, Qt::AlignCenter);

    setLayout(pqvblLayout);
    resize(sizeHint());
    show();
}


void IComDlgCredits::AddLabel(QVBoxLayout* pqvblLayout, const QString & krqstrTitle, const QString & krqstrCredits)
{
    if (krqstrCredits.isEmpty())
        return;

    QLabel* pqlblCredits = new QLabel(QString("<center><h2>%1</h2><p>%2</p></center>").arg(krqstrTitle).arg(krqstrCredits));
    pqlblCredits->setTextFormat(Qt::RichText);
    pqlblCredits->setOpenExternalLinks(true);
    pqvblLayout->addWidget(pqlblCredits);
}
