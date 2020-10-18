#include <QtWidgets>
#include "IDlgSaveRenameSettings.h"
#include "IUIRenameSettings.h"


IDlgSaveRenameSettings::IDlgSaveRenameSettings(IUIRenameSettings* puirsRenameSettingsUI) : QDialog(puirsRenameSettingsUI)
{
    m_puirsRenameSettingsUI = puirsRenameSettingsUI;

    setWindowTitle(tr("Save Rename Settings"));
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QLabel* pqlblName = new QLabel(tr("Name:"));
    m_pqleName = new QLineEdit;
    connect(m_pqleName, SIGNAL(textChanged(const QString &)), this, SLOT(EnableDisableSaveButton()));

    m_pqdbbCancelSave = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Save);
    m_pqdbbCancelSave->button(QDialogButtonBox::Save)->setEnabled(false);
    m_pqdbbCancelSave->setOrientation(Qt::Horizontal);
    connect(m_pqdbbCancelSave, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_pqdbbCancelSave, SIGNAL(rejected()), this, SLOT(reject()));

    QHBoxLayout *qhblSaveLayout = new QHBoxLayout;
    qhblSaveLayout->addWidget(pqlblName);
    qhblSaveLayout->addWidget(m_pqleName);

    QVBoxLayout *qvblLayout = new QVBoxLayout;
    qvblLayout->addLayout(qhblSaveLayout);
    qvblLayout->addWidget(m_pqdbbCancelSave);

    setLayout(qvblLayout);
    resize(300, sizeHint().height());
    show();
}


void IDlgSaveRenameSettings::EnableDisableSaveButton()
{
    m_pqdbbCancelSave->button(QDialogButtonBox::Save)->setEnabled(!m_pqleName->text().isEmpty());
}


void IDlgSaveRenameSettings::accept()
{
    QString qstrName = m_pqleName->text();
    IRenameSave* psrsExistingRenameSettings =  m_puirsRenameSettingsUI->GetRenameSettingsFromName(qstrName);
    if (psrsExistingRenameSettings != nullptr)
    {
        int iAnswer = QMessageBox::information(this,
                                               tr("Overwrite Entry"),
                                               tr("There is already saved rename called \"%1\".\nDo you wish to overwrite it with these settings?").arg(qstrName),
                                               QMessageBox::Yes | QMessageBox::No);
        if (iAnswer == QMessageBox::No)
            return;
    }

    m_puirsRenameSettingsUI->SaveRenameSettings(qstrName, psrsExistingRenameSettings);
    QDialog::accept();
}
