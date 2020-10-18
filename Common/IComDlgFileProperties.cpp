#include <QtWidgets>
#include <QFileIconProvider>
#include "IComDlgFileProperties.h"
#include "IUIMainWindow.h"
#include "IComWdgtMetaExif.h"
#include "IComWdgtMetaMusic.h"

/* Look into using Windows shell file properties with ShellExecute
     http://www.qtcentre.org/threads/30066-Using-ShellExecute
*/


IComDlgFileProperties::IComDlgFileProperties(const QString & krqstrFilePath) : QDialog(IUIMainWindow::GetMainWindow()),
                                                                               m_qfiFileInfo(QDir::fromNativeSeparators(krqstrFilePath))
{
    m_qstrFilePathNat = QDir::toNativeSeparators(krqstrFilePath);
    InitDialog();
}


IComDlgFileProperties::IComDlgFileProperties(const QFileInfo & krqfiFileInfo) : QDialog(IUIMainWindow::GetMainWindow()),
                                                                                m_qfiFileInfo(krqfiFileInfo)
{
    if (m_qfiFileInfo.isAbsolute())
        m_qstrFilePathNat = QDir::toNativeSeparators(m_qfiFileInfo.filePath());
    else
        m_qstrFilePathNat = QDir::toNativeSeparators(m_qfiFileInfo.absoluteFilePath());
    InitDialog();
}


void IComDlgFileProperties::InitDialog()
{
     setWindowTitle(m_qfiFileInfo.fileName());
     setAttribute(Qt::WA_DeleteOnClose);
     setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

     m_qtabwTabWidget = new QTabWidget(this);
     m_qtabwTabWidget->addTab(CreateFilePropertiesWdgt(), tr("General"));
     AddTagsTabs();

     m_pqdbbOK = new QDialogButtonBox(QDialogButtonBox::Ok);
     m_pqdbbOK->setOrientation(Qt::Horizontal);
     connect(m_pqdbbOK, SIGNAL(accepted()), this, SLOT(accept()));

     QVBoxLayout* pqvblLayout = new QVBoxLayout;
     pqvblLayout->addWidget(m_qtabwTabWidget);
     pqvblLayout->addWidget(m_pqdbbOK);
     setLayout(pqvblLayout);
     pqvblLayout->setContentsMargins(6, 6, 6, 6);

     //resize(sizeHint());
     resize(385, 470);
     show();
}


QWidget* IComDlgFileProperties::CreateFilePropertiesWdgt()
{
    /* QFileIconProvider only ever returns a maximum icon size of 32x32 on Windows, even when larger sizes are available
     * Running the below confirms that 32x32 is always the largest available size
     *      qDebug() << "Icon Available Sizes: "<< qfipIconProvider.icon(m_qfiFileInfo).availableSizes();
     * For every icon it just prints:
     *      Icon Available Sizes:  (QSize(14, 14), QSize(32, 32))       */

    // These enums let you easily change the row order and add or remove colums
    enum Rows {FileNameRow, SeparatorName, TypeRow, MIMERow, SeparatorType, LocationRow, SizeRow, SeparatorSize, CreatedRow, ModifedRow, AccessedRow, SeparatorDates, AttribRow1, AttribRow2, AttribRow3};
    enum Columns {LeftCol, SpacerCol, RightCol, FullColSpan};

    QVBoxLayout* pvblLayout = new QVBoxLayout;
    QGridLayout* pqglGrid = new QGridLayout;
    pvblLayout->addLayout(pqglGrid);
    pvblLayout->addStretch();

    pqglGrid->setVerticalSpacing(10);
    pqglGrid->setColumnMinimumWidth(SpacerCol, 20);
    pvblLayout->setContentsMargins(15, 15, 15, 15);

    QFileIconProvider qfipIconProvider;
    QLabel* pqlblIcon = new QLabel;
    pqlblIcon->setPixmap(qfipIconProvider.icon(m_qfiFileInfo).pixmap(32, 32));
    //pqlblIcon->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    pqglGrid->addWidget(pqlblIcon, FileNameRow, LeftCol);

    QLineEdit* pqleFileName = new QLineEdit(m_qfiFileInfo.fileName());
    pqleFileName->setReadOnly(true);
    pqleFileName->setCursorPosition(0);
    pqglGrid->addWidget(pqleFileName, FileNameRow, RightCol);

    pqglGrid->addWidget(CreateSeparatorLine(), SeparatorName, LeftCol, 1, FullColSpan);

    QString qstrMIMEType, qstrTypeDescription;
    GetMIMEType(qstrMIMEType, qstrTypeDescription);
    pqglGrid->addWidget(new QLabel(tr("Type of File:")), TypeRow, LeftCol);
    pqglGrid->addWidget(new QLabel(qstrTypeDescription), TypeRow, RightCol);
    pqglGrid->addWidget(new QLabel(tr("MIME Type:")), MIMERow, LeftCol);
    pqglGrid->addWidget(new QLabel(qstrMIMEType), MIMERow, RightCol);

    pqglGrid->addWidget(CreateSeparatorLine(), SeparatorType, LeftCol, 1, FullColSpan);

    pqglGrid->addWidget(new QLabel(tr("Location:")), LocationRow, LeftCol);
    pqglGrid->addWidget(new QLabel(QDir::toNativeSeparators(m_qfiFileInfo.path())), LocationRow, RightCol);
    pqglGrid->addWidget(new QLabel(tr("Size:")), SizeRow, LeftCol);
    pqglGrid->addWidget(new QLabel(GetSizeString()), SizeRow, RightCol);

    pqglGrid->addWidget(CreateSeparatorLine(), SeparatorSize, LeftCol, 1, FullColSpan);

    pqglGrid->addWidget(new QLabel(tr("Created:")), CreatedRow, LeftCol);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    pqglGrid->addWidget(new QLabel(m_qfiFileInfo.birthTime().toString(Qt::SystemLocaleLongDate)), CreatedRow, RightCol);
#else
    pqglGrid->addWidget(new QLabel(m_qfiFileInfo.created().toString(Qt::SystemLocaleLongDate)), CreatedRow, RightCol);
#endif
    pqglGrid->addWidget(new QLabel(tr("Modifed:")), ModifedRow, LeftCol);
    pqglGrid->addWidget(new QLabel(m_qfiFileInfo.lastModified().toString(Qt::SystemLocaleLongDate)), ModifedRow, RightCol);
    pqglGrid->addWidget(new QLabel(tr("Accessed:")), AccessedRow, LeftCol);
    pqglGrid->addWidget(new QLabel(m_qfiFileInfo.lastRead().toString(Qt::SystemLocaleLongDate)), AccessedRow, RightCol);

    pqglGrid->addWidget(CreateSeparatorLine(), SeparatorDates, LeftCol, 1, FullColSpan);

    #ifdef Q_OS_WIN
    pqglGrid->addWidget(new QLabel(tr("Read Only:")), AttribRow1, LeftCol);
    pqglGrid->addWidget(new QLabel(m_qfiFileInfo.isWritable() == false ? tr("Yes") : tr("No")), AttribRow1, RightCol);
    #else
    char szPermissions[11];
    GetFilePermissionsString(szPermissions);
    pqglGrid->addWidget(new QLabel(tr("Permissions:")), AttribRow1, LeftCol);
    pqglGrid->addWidget(new QLabel(szPermissions), AttribRow1, RightCol);
    #endif
    pqglGrid->addWidget(new QLabel(tr("Executable:")), AttribRow2, LeftCol);
    pqglGrid->addWidget(new QLabel(m_qfiFileInfo.isExecutable() ? tr("Yes") : tr("No")), AttribRow2, RightCol);
    pqglGrid->addWidget(new QLabel(tr("Hidden:")), AttribRow3, LeftCol);
    pqglGrid->addWidget(new QLabel(m_qfiFileInfo.isHidden() ? tr("Yes") : tr("No")), AttribRow3, RightCol);

    QWidget* pqwWidget = new QWidget(this);
    pqwWidget->setLayout(pvblLayout);
    return pqwWidget;
}


QFrame* IComDlgFileProperties::CreateSeparatorLine()
{
    QFrame* pqfLine = new QFrame;
    pqfLine->setFrameShape(QFrame::HLine);
    pqfLine->setFrameShadow(QFrame::Sunken);
    return pqfLine;
}


void IComDlgFileProperties::GetMIMEType(QString & rqstrMIMEType, QString & rqstrTypeDesc)
{
    QMimeDatabase qmdbMimeDatabase;
    QMimeType qmtMimeType = qmdbMimeDatabase.mimeTypeForFile(m_qfiFileInfo);
    rqstrMIMEType = qmtMimeType.name();

    if (m_qfiFileInfo.suffix().isEmpty())
        rqstrTypeDesc = qmtMimeType.comment();
    else
        rqstrTypeDesc = qmtMimeType.comment() + " (." + m_qfiFileInfo.suffix() + ')';

    // The description is often all lower case (e.g. "plain text file") which bothers me, but some formats just return he MIME type, which should be all lower
    // We therefore convert the first letter to upper, so long as it's not a MIME type (i.e. doens't contain a '/')
    if (rqstrTypeDesc.at(0).isLower() && rqstrTypeDesc.contains('/') == false)
        rqstrTypeDesc[0] = rqstrTypeDesc.at(0).toUpper();
}


QString IComDlgFileProperties::GetSizeString()
{
    const int kiKilobyte = 1024;
    const int kiMegabyte = 1048576;
    const int kiGigabyte = 1073741824;
    //const qint64 klliTerabyte = 1099511627776;

    qint64 lliSize = m_qfiFileInfo.size();
    if (lliSize > kiGigabyte)
    {
        return QString("%1 GB (%L2 bytes)").arg(QString::number(static_cast<double>(lliSize) / kiGigabyte, 'f', 2)).arg(lliSize);
    }
    else if (lliSize > kiMegabyte)
    {
        return QString("%1 MB (%L2 bytes)").arg(QString::number(static_cast<double>(lliSize) / kiMegabyte, 'f', 2)).arg(lliSize);
    }
    else if (lliSize > kiKilobyte)
    {
        return QString("%1 KB (%L2 bytes)").arg(QString::number(static_cast<double>(lliSize) / kiKilobyte, 'f', 2)).arg(lliSize);
    }

    return QString("%L1 bytes").arg(lliSize);
}


void IComDlgFileProperties::GetFilePermissionsString(char* szPermissions)
{
    #ifdef Q_OS_WIN
    szPermissions[0] = '\0';
    #else
    szPermissions[0] = m_qfiFileInfo.isDir() ? 'd' : '-';
    szPermissions[1] = m_qfiFileInfo.permission(QFile::ReadOwner)  ? 'r' : '-';
    szPermissions[2] = m_qfiFileInfo.permission(QFile::WriteOwner) ? 'w' : '-';
    szPermissions[3] = m_qfiFileInfo.permission(QFile::ExeOwner)   ? 'e' : '-';
    szPermissions[4] = m_qfiFileInfo.permission(QFile::ReadGroup)  ? 'r' : '-';
    szPermissions[5] = m_qfiFileInfo.permission(QFile::WriteGroup) ? 'w' : '-';
    szPermissions[6] = m_qfiFileInfo.permission(QFile::ExeGroup)   ? 'e' : '-';
    szPermissions[7] = m_qfiFileInfo.permission(QFile::ReadOther)  ? 'r' : '-';
    szPermissions[8] = m_qfiFileInfo.permission(QFile::WriteOther) ? 'w' : '-';
    szPermissions[9] = m_qfiFileInfo.permission(QFile::ExeOther)   ? 'e' : '-';
    szPermissions[10] = '\0';
    #endif
}


void IComDlgFileProperties::AddTagsTabs()
{
    if (IComMetaExif::FileCanContainExif(m_qfiFileInfo.suffix()))
    {
        IComWdgtMetaExif* pwmeExifMetaWidget = IComWdgtMetaExif::CreateWidget(m_qstrFilePathNat, m_qtabwTabWidget);
        if (pwmeExifMetaWidget != nullptr)
        {
            //pwmeExifMetaWidget->setStyleSheet("QTableWidget {border: 0px;}");
            m_qtabwTabWidget->setStyleSheet("QTabWidget::pane {margin: 0px 0px 0px 0px;}");
            m_qtabwTabWidget->addTab(pwmeExifMetaWidget, tr("Exif"));
        }
    }

    IComWdgtMetaMusic* pwmmMusicMetaWidget = IComWdgtMetaMusic::CreateWidget(m_qstrFilePathNat, m_qtabwTabWidget);
    if (pwmmMusicMetaWidget != nullptr)
    {
        //pwmeExifMetaWidget->setStyleSheet("QTableWidget {border: 0px;}");
        m_qtabwTabWidget->setStyleSheet("QTabWidget::pane {margin: 0px 0px 0px 0px;}");
        m_qtabwTabWidget->addTab(pwmmMusicMetaWidget, tr("Media"));
    }
}
