#include <QtWidgets>
#include "IDlgRenameErrorList.h"
#include "IUIFileList.h"


IDlgRenameErrorList::IDlgRenameErrorList(IUIFileList* puifmFileList) : QDialog(puifmFileList)
{
    setWindowTitle(tr("Rename Errors"));
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QLabel* pqlblFailMessage = new QLabel(tr("The following files could not be renamed:"));

    m_pqpbCloseButton = new QPushButton(tr("Close"));
    m_pqpbCloseButton->setDefault(true);
    connect(m_pqpbCloseButton, SIGNAL(clicked()), this, SLOT(close()));

    m_pqtwFileList = new QTableWidget(this);
    m_pqtwFileList->setColumnCount(3);

    QTableWidgetItem* pqtwiHeader = new QTableWidgetItem;
    pqtwiHeader->setText(tr("Current Name"));
    m_pqtwFileList->setHorizontalHeaderItem(0, pqtwiHeader);
    pqtwiHeader = new QTableWidgetItem;
    pqtwiHeader->setText(tr("Failed Rename"));
    m_pqtwFileList->setHorizontalHeaderItem(1, pqtwiHeader);
    pqtwiHeader = new QTableWidgetItem;
    pqtwiHeader->setText(tr("Probable Failure Reason"));
    m_pqtwFileList->setHorizontalHeaderItem(2, pqtwiHeader);

    m_pqtwFileList->setEditTriggers(QAbstractItemView::NoEditTriggers);                 // Items not editable
    m_pqtwFileList->setWordWrap(false);                                                 // Disable word wrapping when contents won't fit in cell.
    m_pqtwFileList->verticalHeader()->hide();                                           // No row lables
    m_pqtwFileList->horizontalHeader()->setSectionsClickable(false);                    // Column headers not clicable
    m_pqtwFileList->horizontalHeader()->setStretchLastSection(true);                    // Streatch last column to fill remaining space

    m_pqtwFileList->setFont(puifmFileList->GetCurrentFont());
    m_pqtwFileList->verticalHeader()->setFont(puifmFileList->GetCurrentFont());

    QHBoxLayout* pqhblCloseLayout = new QHBoxLayout;
    pqhblCloseLayout->addStretch();
    pqhblCloseLayout->addWidget(m_pqpbCloseButton);

    QVBoxLayout* pqvblLayout = new QVBoxLayout;
    pqvblLayout->addWidget(pqlblFailMessage);
    pqvblLayout->addWidget(m_pqtwFileList);
    pqvblLayout->addLayout(pqhblCloseLayout);

    setLayout(pqvblLayout);
}


void IDlgRenameErrorList::AddFileToList(const QString & krqstrCurrentName, const QString & krqstrFailedRename, const QString & krqstrFailureReason)
{
    int iRow = m_pqtwFileList->rowCount();
    m_pqtwFileList->setRowCount(iRow+1);

    QTableWidgetItem* pqtwiItem = new QTableWidgetItem;
    pqtwiItem->setText(krqstrCurrentName);
    m_pqtwFileList->setItem(iRow, 0, pqtwiItem);

    pqtwiItem = new QTableWidgetItem;
    pqtwiItem->setText(krqstrFailedRename);
    m_pqtwFileList->setItem(iRow, 1, pqtwiItem);

    pqtwiItem = new QTableWidgetItem;
    pqtwiItem->setText(krqstrFailureReason);
    m_pqtwFileList->setItem(iRow, 2, pqtwiItem);
}


void IDlgRenameErrorList::ResizeColumnsAndShow()
{
    m_pqtwFileList->resizeColumnsToContents();
    m_pqtwFileList->setColumnWidth(0, m_pqtwFileList->columnWidth(0)+20);
    m_pqtwFileList->setColumnWidth(1, m_pqtwFileList->columnWidth(1)+20);

    int iWidth = m_pqtwFileList->columnWidth(0) + m_pqtwFileList->columnWidth(1) + m_pqtwFileList->columnWidth(2);
    resize(iWidth+50, 400);
    show();
}
