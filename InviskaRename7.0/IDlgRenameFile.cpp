#include <QtWidgets>
#include "IDlgRenameFile.h"
#include "IUIFileList.h"


IDlgRenameFile::IDlgRenameFile(IUIFileList* puifmFileList, QDir & rqdirDirReader, const QString & krqstrFileName) : QDialog(puifmFileList),
                                                                                                                    m_rqdirDirReader(rqdirDirReader)
{
    m_qstrOriginalFileName = krqstrFileName;

    setWindowTitle(tr("Rename File"));
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QLabel* pqlblNewName = new QLabel(tr("New Name:"));
    m_pqleNewName = new QLineEdit(krqstrFileName);
    connect(m_pqleNewName, SIGNAL(textChanged(const QString &)), this, SLOT(FileNameTextChanged()));

    int iExtenIndex = krqstrFileName.lastIndexOf('.');
    if (iExtenIndex != -1)
        m_pqleNewName->setCursorPosition(krqstrFileName.lastIndexOf('.'));

    #ifdef Q_OS_WIN
    m_pqrevInvalidCharValidator = new QRegExpValidator(QRegExp("^[^\\\\/:*?\"<>|]*$"), this);     // Windows    \/:*?"<>|
    #else
    m_pqrevInvalidCharValidator = new QRegExpValidator(QRegExp("^[^/]*$"), this);                 // Linux+Mac  /
    #endif
    m_pqleNewName->setValidator(m_pqrevInvalidCharValidator);

    QFontMetrics qfmFontMetrics(m_pqleNewName->font());
    m_iNamePadding = qfmFontMetrics.width('W');

    m_pqdbbCancelRename = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    m_pqdbbCancelRename->button(QDialogButtonBox::Ok)->setEnabled(false);
    m_pqdbbCancelRename->button(QDialogButtonBox::Ok)->setText("Rename");
    m_pqdbbCancelRename->setOrientation(Qt::Horizontal);
    connect(m_pqdbbCancelRename, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_pqdbbCancelRename, SIGNAL(rejected()), this, SLOT(reject()));

    QHBoxLayout* qhblNewNameLayout = new QHBoxLayout;
    qhblNewNameLayout->addWidget(pqlblNewName);
    qhblNewNameLayout->addWidget(m_pqleNewName);

    QVBoxLayout* qvblLayout = new QVBoxLayout;
    qvblLayout->addLayout(qhblNewNameLayout);
    qvblLayout->addWidget(m_pqdbbCancelRename);

    setLayout(qvblLayout);
    resize(sizeHint());
    show();

    FileNameTextChanged();
}


void IDlgRenameFile::FileNameTextChanged()
{
    QString qstrNewName = m_pqleNewName->text();

    if (m_qstrOriginalFileName != qstrNewName && qstrNewName.isEmpty() == false)
        m_pqdbbCancelRename->button(QDialogButtonBox::Ok)->setEnabled(true);
    else
        m_pqdbbCancelRename->button(QDialogButtonBox::Ok)->setEnabled(false);

    QFontMetrics qfmFontMetrics(m_pqleNewName->font());
    int iWidth = qfmFontMetrics.width(qstrNewName) + m_iNamePadding;
    m_pqleNewName->setMinimumWidth(iWidth);
}


void IDlgRenameFile::accept()
{
    QString qstrNewFileName = m_pqleNewName->text();
    if (m_rqdirDirReader.exists(qstrNewFileName))
    {
        QMessageBox::warning(this, tr("File Already Exists"), tr("A file with the specified name already exists.\nPlease enter an alternative name."), QMessageBox::Ok);
        return;
    }

    if (m_rqdirDirReader.rename(m_qstrOriginalFileName, qstrNewFileName) == false)
    {
        if (m_rqdirDirReader.exists(m_qstrOriginalFileName) == false)
        {
           QMessageBox::critical(this, tr("File No Longer Exists"), tr("Rename failed because the original file no longer exists.\nIt may have been deleted, moved or renamed."), QMessageBox::Ok);
           QDialog::accept();
        }
        else
        {
            int iAnswer = QMessageBox::warning(this, tr("Rename Filure"), tr("File may be open in other application or you may not have permission to modify the file."), QMessageBox::Abort | QMessageBox::Retry);
            while (iAnswer == QMessageBox::Retry)
            {
                if (m_rqdirDirReader.rename(m_qstrOriginalFileName, qstrNewFileName) == false)
                    iAnswer = QMessageBox::warning(this, tr("Rename Filure"), tr("File may be open in other application or you may not have permission to modify the file."), QMessageBox::Abort | QMessageBox::Retry);
                else
                    break;
            }
        }
    }

    QDialog::accept();
}
