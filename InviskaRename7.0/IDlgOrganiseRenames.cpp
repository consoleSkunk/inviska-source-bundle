#include <QtWidgets>
#include "IDlgOrganiseRenames.h"
#include "IUIRenameSettings.h"
#include "IRenameSave.h"


IDlgOrganiseRenames::IDlgOrganiseRenames(IUIRenameSettings* puirsRenameSettingsUI) : QDialog(puirsRenameSettingsUI)
{
    m_puirsRenameSettingsUI = puirsRenameSettingsUI;

    setWindowTitle(tr("Organise Saved Renames"));
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    m_pqlwRenames = new QListWidget(this);
    PopulateRenamesList();

    m_qpbMoveUp   = new QPushButton(tr("Move Up"), this);
    m_qpbMoveDown = new QPushButton(tr("Move Down"), this);
    m_qpbDelete   = new QPushButton(tr("Delete"), this);
    connect(m_qpbMoveUp,   SIGNAL(clicked()), this, SLOT(MoveUp()));
    connect(m_qpbMoveDown, SIGNAL(clicked()), this, SLOT(MoveDown()));
    connect(m_qpbDelete,   SIGNAL(clicked()), this, SLOT(Delete()));

    m_qdbbOKCancel = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok );
    m_qdbbOKCancel->setOrientation(Qt::Vertical);
    connect(m_qdbbOKCancel, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_qdbbOKCancel, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *qvblButtonLayout = new QVBoxLayout;
    qvblButtonLayout->addWidget(m_qpbMoveUp);
    qvblButtonLayout->addWidget(m_qpbMoveDown);
    qvblButtonLayout->addWidget(m_qpbDelete);
    qvblButtonLayout->addStretch(1);
    qvblButtonLayout->addWidget(m_qdbbOKCancel);

    QHBoxLayout *qhblLayout = new QHBoxLayout;
    qhblLayout->addWidget(m_pqlwRenames);
    qhblLayout->addLayout(qvblButtonLayout);

    setLayout(qhblLayout);
    resize(sizeHint());
    show();
}


void IDlgOrganiseRenames::PopulateRenamesList()
{
    QListWidgetItem* pqlwiNewItem;
    const QVector<IRenameSave*> & krqvecpsrenSavedRenames = m_puirsRenameSettingsUI->GetSavedRenameSettings();
    QVector<IRenameSave*>::const_iterator kitSavedRename;
    for (kitSavedRename = krqvecpsrenSavedRenames.constBegin() ; kitSavedRename < krqvecpsrenSavedRenames.constEnd() ; ++kitSavedRename)
    {
        pqlwiNewItem = new QListWidgetItem((*kitSavedRename)->m_qstrName);
        pqlwiNewItem->setData(Qt::UserRole, QVariant::fromValue(*kitSavedRename));
        m_pqlwRenames->addItem(pqlwiNewItem);
    }
}


void IDlgOrganiseRenames::MoveUp()
{
    int iSelectedRow = m_pqlwRenames->currentRow();
    if (m_pqlwRenames->count() == 0 || iSelectedRow == 0)
        return;

    QListWidgetItem* pqlwiItem = m_pqlwRenames->takeItem(iSelectedRow);
    m_pqlwRenames->insertItem(--iSelectedRow, pqlwiItem);
    m_pqlwRenames->setCurrentRow(iSelectedRow);
}


void IDlgOrganiseRenames::MoveDown()
{
    int iSelectedRow = m_pqlwRenames->currentRow();
    if (m_pqlwRenames->count() == 0 || iSelectedRow == m_pqlwRenames->count()-1)
        return;

    QListWidgetItem* pqlwiItem = m_pqlwRenames->takeItem(iSelectedRow);
    m_pqlwRenames->insertItem(++iSelectedRow, pqlwiItem);
    m_pqlwRenames->setCurrentRow(iSelectedRow);
}


void IDlgOrganiseRenames::Delete()
{
    if (m_pqlwRenames->count() == 0)
        return;

    QListWidgetItem* pqlwiItem = m_pqlwRenames->takeItem(m_pqlwRenames->currentRow());
    m_qvecpsrenRenamesToDelete.push_back(pqlwiItem->data(Qt::UserRole).value<IRenameSave*>());
    delete pqlwiItem;
}


void IDlgOrganiseRenames::accept()
{
    if (m_qvecpsrenRenamesToDelete.empty() == false)
    {
        QVector<IRenameSave*>::iterator itRename;
        for (itRename = m_qvecpsrenRenamesToDelete.begin() ; itRename < m_qvecpsrenRenamesToDelete.end() ; ++itRename)
            delete *itRename;
    }

    m_puirsRenameSettingsUI->ReadRenamesFromListWidget(m_pqlwRenames);

    QDialog::accept();
}
