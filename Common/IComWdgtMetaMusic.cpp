#include <QtWidgets>
#include "IComWdgtMetaMusic.h"
#include "IComMetaMusic.h"
#include "IComUtilityFuncs.h"
#include "IComQLineEdit.h"
#include "IUIMainWindow.h"

/* I just wanted to explain the somewhat bizzare implementation of this UI.
 * Initially it just used a QTableWidget, but I wanted to add a QIntValidator to the Track and Year value cells.
 * QTableWidget item does not support validators, so I put a QLineEdit in the Track and Year cells and added the QIntValidator to them.
 * That worked, but the cells with the QLineEdit had a slightly different appearance and behaviour to the other cells.
 * For consistency I changed all the editable cells to use QLineEdits, which gave a consistent bahaviour and appearance.
 * However, tab behaviour seemed somewhat odd, with you having to press tab twice to tab between cells, and the tab order felt unnatural.
 * I tried setting the tab order with setTabOrder(m_pqletTitle, m_pqletArtist) etc, but this order was ignored.
 * I then tried reimplementing focusNextPrevChild but when the QLineEdits have focus currentRow/Column() always return -1, and currentItem() always nullptr.
 * Finally I created an array of QLineEditTags which define the tab order and reimplemented focusNextPrevChild to cycle through them.
 * The UI utimately ended up with a strange implemention with numerous extra widgets where one would expect a QTableWidget alone would suffice.
 * However, this was the only way to get a consistent appearance which behaved as you would expect. */


IComWdgtMetaMusic* IComWdgtMetaMusic::CreateWidget(const QString & krqstrFilePath, QWidget* pqwParent)
{   
    IComWdgtMetaMusic* pwmmMusicMetaWidget = nullptr;
    IComMetaMusic mmuMusicMeta(krqstrFilePath);

    if (mmuMusicMeta.TagDataPresent())
        pwmmMusicMetaWidget = new IComWdgtMetaMusic(krqstrFilePath, mmuMusicMeta, pqwParent);

    return pwmmMusicMetaWidget;
}


IComWdgtMetaMusic::IComWdgtMetaMusic(const QString & krqstrFilePath, IComMetaMusic & rmmuMusicMeta, QWidget* pqwParent) : QTableWidget(pqwParent)
{
    m_qstrFilePath = QDir::fromNativeSeparators(krqstrFilePath);
    m_pivalIntOnlyValidator = new QIntValidator(this);

    setColumnCount(2);
    setHorizontalHeaderLabels(QStringList() << tr("Tag") << tr("Value"));
    setStyleSheet("QTableWidget::item { padding: 0px; }");
    verticalHeader()->setMinimumSectionSize(17);
    verticalHeader()->setDefaultSectionSize(IComUtilityFuncs::GetTableRowHeightFitToFont(this));
    verticalHeader()->hide();

    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    horizontalHeader()->setSectionsClickable(false);


    setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_pqpbApplyChanges = new QPushButton(tr("Apply Changes"), this);
    m_pqpbApplyChanges->setEnabled(false);
    connect(m_pqpbApplyChanges, SIGNAL(clicked()), this, SLOT(SaveUpdatedTags()));

    PopulateTable(rmmuMusicMeta);
}


void IComWdgtMetaMusic::PopulateTable(IComMetaMusic & rmmuMusicMeta)
{
    PopulateTableAddDivision(tr("Tag Info"));
    PopulateTableAddItem(tr("Title"),       rmmuMusicMeta.GetTitle(),    m_pqletTitle,    false);
    PopulateTableAddItem(tr("Artist"),      rmmuMusicMeta.GetArtist(),   m_pqletArtist,   false);
    PopulateTableAddItem(tr("Album"),       rmmuMusicMeta.GetAlbum(),    m_pqletAlbum,    false);
    PopulateTableAddItem(tr("Track"),       rmmuMusicMeta.GetTrack(),    m_pqletTrack,    true);
    PopulateTableAddItem(tr("Year"),        rmmuMusicMeta.GetYear(),     m_pqletYear,     true);
    PopulateTableAddItem(tr("Genre"),       rmmuMusicMeta.GetGenre(),    m_pqletGenre,    false);
    PopulateTableAddItem(tr("Comment"),     rmmuMusicMeta.GetComment(),  m_pqletComment,  false);
    PopulateTableAddApplyButton();

    PopulateTableAddDivision(tr("Media Info"));
    PopulateTableAddItem(tr("Length"),      rmmuMusicMeta.GetLengthMMSSColon());
    PopulateTableAddItem(tr("Channels"),    rmmuMusicMeta.GetChannels());
    PopulateTableAddItem(tr("Sample Rate"), rmmuMusicMeta.GetSampleRateHz());
    PopulateTableAddItem(tr("Bit Rate"),    rmmuMusicMeta.GetBitRateSlash());
}


void IComWdgtMetaMusic::PopulateTableAddDivision(const QString & krqstrTitle)
{
    const int kiRowCount = rowCount();
    insertRow(kiRowCount);

    QTableWidgetItem* pqtwiItem;
    pqtwiItem = new QTableWidgetItem(krqstrTitle);
    pqtwiItem->setBackground(QColor::fromRgb(192, 192, 192));
    pqtwiItem->setFlags(pqtwiItem->flags() ^ (Qt::ItemIsEditable | Qt::ItemIsSelectable));
    setItem(kiRowCount, 0, pqtwiItem);

    pqtwiItem = new QTableWidgetItem;
    pqtwiItem->setBackground(QColor::fromRgb(192, 192, 192));
    pqtwiItem->setFlags(pqtwiItem->flags() ^ (Qt::ItemIsEditable | Qt::ItemIsSelectable));
    setItem(kiRowCount, 1, pqtwiItem);
}


void IComWdgtMetaMusic::PopulateTableAddItem(const QString & krqstrTag, const QString & krqstrValue)
{
    const int kiRowCount = rowCount();
    insertRow(kiRowCount);

    QTableWidgetItem* pqtwiItem;
    pqtwiItem = new QTableWidgetItem(krqstrTag);
    pqtwiItem->setFlags(pqtwiItem->flags() ^ (Qt::ItemIsEditable | Qt::ItemIsSelectable));
    setItem(kiRowCount, 0, pqtwiItem);

    pqtwiItem = new QTableWidgetItem(krqstrValue);
    pqtwiItem->setFlags(pqtwiItem->flags() ^ (Qt::ItemIsEditable | Qt::ItemIsSelectable));
    setItem(kiRowCount, 1, pqtwiItem);
}


void IComWdgtMetaMusic::PopulateTableAddItem(const QString & krqstrTag, const QString & krqstrValue, QLineEditTag* & rpqletLineEdit, bool bIntOnly)
{
    const int kiRowCount = rowCount();
    insertRow(kiRowCount);

    QTableWidgetItem* pqtwiItem;
    pqtwiItem = new QTableWidgetItem(krqstrTag);
    pqtwiItem->setFlags(pqtwiItem->flags() ^ (Qt::ItemIsEditable | Qt::ItemIsSelectable));
    setItem(kiRowCount, 0, pqtwiItem);

    rpqletLineEdit = new QLineEditTag(this);
    rpqletLineEdit->m_bModified = false;
    rpqletLineEdit->setFrame(false);
    rpqletLineEdit->setText(krqstrValue);
    rpqletLineEdit->setCursorPosition(0);
    if (bIntOnly)
        rpqletLineEdit->setValidator(m_pivalIntOnlyValidator);
    connect(rpqletLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(TagValueModified()));

    m_rgpqletLineEdits[kiRowCount-1] = rpqletLineEdit;

    setCellWidget(kiRowCount, 1, rpqletLineEdit);
}


void IComWdgtMetaMusic::PopulateTableAddApplyButton()
{
    const int kiRowCount = rowCount();
    insertRow(kiRowCount);

    QTableWidgetItem* pqtwiItem;
    pqtwiItem = new QTableWidgetItem();
    pqtwiItem->setFlags(pqtwiItem->flags() ^ (Qt::ItemIsEditable | Qt::ItemIsSelectable));
    setItem(kiRowCount, 0, pqtwiItem);

    setCellWidget(kiRowCount, 1, m_pqpbApplyChanges);

    resizeRowToContents(kiRowCount);
}


void IComWdgtMetaMusic::DisableEditing()
{
    m_pqletTitle->setEnabled(false);
    m_pqletArtist->setEnabled(false);
    m_pqletAlbum->setEnabled(false);
    m_pqletTrack->setEnabled(false);
    m_pqletYear->setEnabled(false);
    m_pqletGenre->setEnabled(false);
    m_pqletComment->setEnabled(false);
    m_pqpbApplyChanges->setEnabled(false);
}


bool IComWdgtMetaMusic::focusNextPrevChild(bool bNext)
{
    for (int iIndex = 0 ; iIndex < m_kiNumLineEdits ; ++iIndex)
    {
        if (m_rgpqletLineEdits[iIndex]->hasFocus())
        {
            if (bNext)
            {
                if (iIndex+1 == m_kiNumLineEdits)
                    m_rgpqletLineEdits[0]->setFocus();
                else
                    m_rgpqletLineEdits[iIndex+1]->setFocus();
            }
            else
            {
                if (iIndex == 0)
                    m_rgpqletLineEdits[m_kiNumLineEdits-1]->setFocus();
                else
                    m_rgpqletLineEdits[iIndex-1]->setFocus();
            }
            return true;
        }
    }
    m_rgpqletLineEdits[0]->setFocus();
    return true;}


void IComWdgtMetaMusic::TagValueModified()
{
    QLineEditTag* pqletLineEdit = qobject_cast<QLineEditTag*>(sender());
    pqletLineEdit->m_bModified = true;
    m_pqpbApplyChanges->setEnabled(true);
}


void IComWdgtMetaMusic::SaveUpdatedTags()
{
    IComMetaMusic mmuMusicMeta(m_qstrFilePath);
    ReadModifiedValues(mmuMusicMeta);
    bool bSuccess = mmuMusicMeta.SaveTagChanges();

    if (bSuccess == false)
    {
        if (SaveFileExists() == false)
            return;

        if (SaveTagDataPresent(mmuMusicMeta) == false)
            return;

        bSuccess = SaveRetrySave(mmuMusicMeta);
    }

    if (bSuccess)
    {
        ClearModifiedFlags();
        m_pqpbApplyChanges->setEnabled(false);
    }
}


bool IComWdgtMetaMusic::SaveFileExists()
{
    if (QFileInfo::exists(m_qstrFilePath))
        return true;

    QMessageBox::critical(IUIMainWindow::GetMainWindow(),
                          tr("Failed To Save Tag Metadata"),
                          tr("Could not save tag data because the file no longer exists."),
                          QMessageBox::Ok);
    DisableEditing();
    return false;
}


bool IComWdgtMetaMusic::SaveTagDataPresent(IComMetaMusic & rmmuMusicMeta)
{
    if (rmmuMusicMeta.TagDataPresent())
        return true;

    QMessageBox::critical(IUIMainWindow::GetMainWindow(),
                          tr("Failed To Save Tag Metadata"),
                          tr("Could not save tag data because tag data could not be accessed."),
                          QMessageBox::Ok);
    DisableEditing();
    return false;
}


bool IComWdgtMetaMusic::SaveRetrySave(IComMetaMusic & rmmuMusicMeta)
{
    int iResponse;
    bool bSuccess = false;
    while (bSuccess == false)
    {
        iResponse = QMessageBox::critical(IUIMainWindow::GetMainWindow(),
                                          tr("Failed To Save Tag Metadata"),
                                          tr("An error occurred while attempting to save the tag data.\n"
                                             "If the file is open in another application, try closing the application and press Retry."),
                                          QMessageBox::Retry | QMessageBox::Cancel);

        if (iResponse == QMessageBox::Cancel)
            return false;

        // At this point the most likely reason for failure is that the file was open in another application and has been opened in read-only mode by TagLib - rmmuMusicMeta.IsReadOnly()
        // The TagLib documenation says that FileRef::save() will try to re-open the file in read-write mode if it's in read-only mode, but it doesn't so we re-open it manually.
        if (rmmuMusicMeta.Reopen(m_qstrFilePath) == false)
        {
            SaveCriticalError();
            return false;
        }
        ReadModifiedValues(rmmuMusicMeta);
        bSuccess = rmmuMusicMeta.SaveTagChanges();
    }
    return true;
}


void IComWdgtMetaMusic::SaveCriticalError()
{
    QMessageBox::critical(IUIMainWindow::GetMainWindow(),
                          tr("Failed To Save Tag Metadata"),
                          tr("Critical failure saving tags.\nTag editing will be disabled.\nTry closing and reopening dialog box."),
                          QMessageBox::Ok);
    DisableEditing();
}


void IComWdgtMetaMusic::ReadModifiedValues(IComMetaMusic & rmmuMusicMeta)
{
    if (m_pqletTitle->m_bModified)
        rmmuMusicMeta.SetTitle(m_pqletTitle->text());
    if (m_pqletArtist->m_bModified)
        rmmuMusicMeta.SetArtist(m_pqletArtist->text());
    if (m_pqletAlbum->m_bModified)
        rmmuMusicMeta.SetAlbum(m_pqletAlbum->text());
    if (m_pqletTrack->m_bModified)
        rmmuMusicMeta.SetTrack(m_pqletTrack->text());
    if (m_pqletYear->m_bModified)
        rmmuMusicMeta.SetYear(m_pqletYear->text());
    if (m_pqletGenre->m_bModified)
        rmmuMusicMeta.SetGenre(m_pqletGenre->text());
    if (m_pqletComment->m_bModified)
        rmmuMusicMeta.SetComment(m_pqletComment->text());
}


void IComWdgtMetaMusic::ClearModifiedFlags()
{
    m_pqletTitle->m_bModified = false;
    m_pqletArtist->m_bModified = false;
    m_pqletAlbum->m_bModified = false;
    m_pqletTrack->m_bModified = false;
    m_pqletYear->m_bModified = false;
    m_pqletGenre->m_bModified = false;
    m_pqletComment->m_bModified = false;
}


void IComWdgtMetaMusic::DisplayTags(const QString & krqstrFilePath)
{
    IComWdgtMetaMusic* pmmwMusicMetaWidget = IComWdgtMetaMusic::CreateWidget(krqstrFilePath);
    if (pmmwMusicMetaWidget == nullptr)
        return;

    QString qstrFileName = QDir::fromNativeSeparators(krqstrFilePath);
    qstrFileName = qstrFileName.mid(qstrFileName.lastIndexOf('/')+1);

    QDialog* pqdlgDialog = new QDialog(IUIMainWindow::GetMainWindow());
    pqdlgDialog->setWindowTitle(qstrFileName + tr(" - Tag Info"));
    pqdlgDialog->setWindowFlags(pqdlgDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    pqdlgDialog->setModal(true);
    pqdlgDialog->resize(400, 600);

    QPushButton* pqpbOkay = new QPushButton(tr("OK"), pqdlgDialog);
    pqpbOkay->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    pqdlgDialog->connect(pqpbOkay, SIGNAL(clicked()), pqdlgDialog, SLOT(close()));

    QVBoxLayout* pqvblLayout = new QVBoxLayout;
    pqvblLayout->addWidget(pmmwMusicMetaWidget);
    pqvblLayout->addWidget(pqpbOkay);
    pqvblLayout->setAlignment(pqpbOkay, Qt::AlignCenter);
    pqdlgDialog->setLayout(pqvblLayout);

    pqdlgDialog->exec();
    delete pqdlgDialog;
}
