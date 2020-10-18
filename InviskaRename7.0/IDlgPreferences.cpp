#include <QtWidgets>
#include "IDlgPreferences.h"
#include "IUIMainWindow.h"
#include "IUIFileList.h"
#include "IComUIPrefGeneral.h"
#include "IComUtilityFuncs.h"
#include "IComQLineEdit.h"

int IDlgPreferences::m_iStartPage = 0;


IDlgPreferences::IDlgPreferences(IUIMainWindow* pmwMainWindow, const int kiStartPage) : QDialog(pmwMainWindow),
                                                                                        m_rqsetSettings(pmwMainWindow->GetSettings())
{
    m_pmwMainWindow = pmwMainWindow;

    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setModal(true);

    m_pipgGeneralPreferences = new IComUIPrefGeneral(this, pmwMainWindow);
    m_pqswPageStack->insertWidget(0, m_pipgGeneralPreferences);

    if (kiStartPage != -1)
        m_iStartPage = kiStartPage;
    m_pqlwPageList->setCurrentRow(m_iStartPage);
    m_pqswPageStack->setCurrentIndex(m_iStartPage);

    #if   defined(Q_OS_WIN)
    m_pqrbStartDirectoryRoot->setText("Computer");
    m_pqrbStartDirectoryHome->setText("User Directory");
    #elif defined(Q_OS_MACOS)
    m_pqrbStartDirectoryRoot->setText("Root");
    m_pqrbStartDirectoryHome->setText("User Directory");
    #else
    m_pqrbStartDirectoryRoot->setText("Root");
    m_pqrbStartDirectoryHome->setText("Home");
    #endif
    m_pqpbStartDirectorySelect->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_DirOpenIcon));

    m_pqsiInvCharTableSpacer->changeSize(0, m_pqcbReSortFileListAfterRename->sizeHint().height(), QSizePolicy::Fixed,  QSizePolicy::Fixed);
    m_pqpbCharSubRestoreDefaults->setFixedSize(m_pqpbCharSubSetDefaultsWin->sizeHint());
    #ifdef Q_OS_WIN
    m_pqpbCharSubSetDefaultsWin->hide();
    #endif

    InitCharSubTable();
    SetWidgetStates();

    connect(m_pqpbNameChangeTextColour,     SIGNAL(clicked()),  this,   SLOT(SetColourChangesColour()));
    connect(m_pqpbNameChangeHighlightColour,SIGNAL(clicked()),  this,   SLOT(SetHighlightChangesColour()));
    connect(m_pqpbFontSelectButton,         SIGNAL(clicked()),  this,   SLOT(SetFileListFont()));

    connect(m_pqpbStartDirectorySelect,     SIGNAL(clicked()),  this,   SLOT(SetCustomStartDirectory()));

    connect(m_pqpbCharSubRestoreDefaults,   SIGNAL(clicked()),  this,   SLOT(PopulateCharSubTableDefault()));
    connect(m_pqpbCharSubSetDefaultsWin,    SIGNAL(clicked()),  this,   SLOT(PopulateCharSubTableDefaultWindows()));

    resize(sizeHint());
    show();
}


void IDlgPreferences::InitCharSubTable()
{
    m_pqtwInvalidCharacterTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_pqtwInvalidCharacterTable->horizontalHeader()->setSectionsClickable(false);
    m_pqtwInvalidCharacterTable->horizontalHeader()->setVisible(true);
    m_pqtwInvalidCharacterTable->verticalHeader()->setMinimumSectionSize(16);
    m_pqtwInvalidCharacterTable->verticalHeader()->setDefaultSectionSize(IComUtilityFuncs::GetTableRowHeightFitToFont(m_pqtwInvalidCharacterTable));
    m_pqtwInvalidCharacterTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_pqtwInvalidCharacterTable->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    #ifdef Q_OS_WIN
    QRegExpValidator* pqrevInvalidCharValidator = new QRegExpValidator(QRegExp("^[^\\\\/:*?\"<>|]*$"), this);     // Windows    \/:*?"<>|
    #else
    QRegExpValidator* pqrevInvalidCharValidator = new QRegExpValidator(QRegExp("^[^/]*$"), this);                 // Linux+Mac  /
    #endif

    const int kiNumInvalidChars = m_pmwMainWindow->GetFileListUI()->GetInvCharSub().GetNumInvalidChars();
    m_pqtwInvalidCharacterTable->setRowCount(kiNumInvalidChars);

    IComQLineEdit* pqleLineEdit;
    QTableWidgetItem* pqleInvCharItem;
    for (int iIndex = 0 ; iIndex < kiNumInvalidChars ; ++iIndex)
    {
        pqleInvCharItem = new QTableWidgetItem;
        pqleInvCharItem->setTextAlignment(Qt::AlignCenter);
        m_pqtwInvalidCharacterTable->setItem(iIndex, 0, pqleInvCharItem);
        m_pqtwInvalidCharacterTable->setItem(iIndex, 1, new QTableWidgetItem);

        pqleLineEdit = new IComQLineEdit(this, true);
        pqleLineEdit->setProperty("Row", iIndex);
        pqleLineEdit->setFrame(false);
        pqleLineEdit->setValidator(pqrevInvalidCharValidator);
        pqleLineEdit->setAlignment(Qt::AlignCenter);
        m_pqtwInvalidCharacterTable->setCellWidget(iIndex, 1, pqleLineEdit);

        #ifndef Q_OS_WIN
        connect(pqleLineEdit, SIGNAL(textChanged(QString)), this, SLOT(CharSubTableCellChanged()));
        #endif
    }

    m_pmwMainWindow->GetFileListUI()->GetInvCharSub().PopulateTableInvalidChars(m_pqtwInvalidCharacterTable);
    m_pmwMainWindow->GetFileListUI()->GetInvCharSub().PopulateTableReplacements(m_pqtwInvalidCharacterTable);
}


void IDlgPreferences::SetWidgetStates()
{
    m_pipgGeneralPreferences->SetWidgetStates();

    IUIFileList* & rpuifmFileList = m_pmwMainWindow->GetFileListUI();

    m_rqsetSettings.beginGroup("Application");
    m_pqrbStartDirectoryRoot->setChecked(m_rqsetSettings.value("StartDirectoryRoot", StartDirectoryRootDefaultVal()).toBool());
    m_pqrbStartDirectoryHome->setChecked(m_rqsetSettings.value("StartDirectoryHome", StartDirectoryHomeDefaultVal()).toBool());
    m_pqrbStartDirectoryCustom->setChecked(m_rqsetSettings.value("StartDirectorySpecified", false).toBool());
    if (m_pqrbStartDirectoryCustom->isChecked())
        m_pqleStartDirectory->setText(m_rqsetSettings.value("StartDirectory", "").toString());
    m_rqsetSettings.endGroup();

    m_pqcbAutoRefreshEnabled->setChecked(rpuifmFileList->AutoRefreshEnabled());
    m_pqcbOpenFileWhenDblClicked->setChecked(rpuifmFileList->OpenFileWhenDblClicked());
    m_qcolNameChangeTextColour = rpuifmFileList->GetNameChangeTextColour();
    m_qcolNameChangeHighlightColour = rpuifmFileList->GetNameChangeHighlightColour();
    m_pqcbNameChangeColourText->setChecked(rpuifmFileList->GetNameChangeColourText());
    m_pqpbNameChangeTextColour->setStyleSheet(QString("QPushButton { background-color : %1;}").arg(m_qcolNameChangeTextColour.name()));
    m_pqcbNameChangeHighlightRow->setChecked(rpuifmFileList->GetNameChangeHighlightRow());
    m_pqpbNameChangeHighlightColour->setStyleSheet(QString("QPushButton { background-color : %1;}").arg(m_qcolNameChangeHighlightColour.name()));

    m_pqcbUseCustomLineSpacing->setChecked(rpuifmFileList->GetUseUserDefinedRowHeight());
    m_pqleCustomLineSpacingPxl->setText(QString::number(rpuifmFileList->GetCurrentRowHeight()));
    m_pqcbUseAlternativeFont->setChecked(rpuifmFileList->GetUseAlternativeFont());
    SetCurrentFont(rpuifmFileList->GetCurrentFont());

    m_pqcbShowConfirmBeforeRename->setChecked(rpuifmFileList->ShowConfirmBeforeRename());
    m_pqcbDeactivateSettingsAfterRename->setChecked(rpuifmFileList->DeactivateSettingsAfterRename());
    m_pqcbReSortFileListAfterRename->setChecked(rpuifmFileList->ReSortFileListAfterRename());

    m_pqcbShowSideBar->setChecked(m_pmwMainWindow->SideBarEnabled());
}


void IDlgPreferences::SetCurrentFont(const QFont & krqfntFont)
{
    m_qfntSelectedFont = krqfntFont;
    m_pqpbFontSelectButton->setFont(krqfntFont);
    m_pqpbFontSelectButton->setText(QString("%1 %2 %3 %4").arg(krqfntFont.family())
                                                          .arg(krqfntFont.pointSize())
                                                          .arg(krqfntFont.bold() ? "Bold" : "")
                                                          .arg(krqfntFont.italic() ? "Italic" : ""));
}


bool IDlgPreferences::SaveSettings()
{
    if (m_pipgGeneralPreferences->SaveSettings() == false)
        return false;

    m_rqsetSettings.beginGroup("Application");
    m_rqsetSettings.setValue("StartDirectoryRoot", m_pqrbStartDirectoryRoot->isChecked());
    m_rqsetSettings.setValue("StartDirectoryHome", m_pqrbStartDirectoryHome->isChecked());
    m_rqsetSettings.setValue("StartDirectorySpecified", m_pqrbStartDirectoryCustom->isChecked());
    m_rqsetSettings.setValue("StartDirectory", m_pqleStartDirectory->text());
    m_rqsetSettings.endGroup();

    m_rqsetSettings.beginGroup("FileList");
    m_rqsetSettings.setValue("AutoRefreshDirectories", m_pqcbAutoRefreshEnabled->isChecked());
    m_rqsetSettings.setValue("OpenFileWhenDblClicked", m_pqcbOpenFileWhenDblClicked->isChecked());
    m_rqsetSettings.setValue("NameChangeColourText", m_pqcbNameChangeColourText->isChecked());
    m_rqsetSettings.setValue("NameChangeTextColour", m_qcolNameChangeTextColour.name());
    m_rqsetSettings.setValue("NameChangeHighlightRow", m_pqcbNameChangeHighlightRow->isChecked());
    m_rqsetSettings.setValue("NameChangeHighlightColour", m_qcolNameChangeHighlightColour.name());
    m_rqsetSettings.setValue("UseUserDefinedRowHeight", m_pqcbUseCustomLineSpacing->isChecked());
    m_rqsetSettings.setValue("UserDefinedRowHeight", m_pqleCustomLineSpacingPxl->text().toInt());
    m_rqsetSettings.setValue("UseAlternativeFont", m_pqcbUseAlternativeFont->isChecked());
    m_rqsetSettings.setValue("FileListFont", m_qfntSelectedFont.toString());
    m_rqsetSettings.endGroup();

    m_rqsetSettings.beginGroup("Rename");
    m_rqsetSettings.setValue("ShowConfirmBeforeRename", m_pqcbShowConfirmBeforeRename->isChecked());
    m_rqsetSettings.setValue("DeactivateSettingsAfterRename", m_pqcbDeactivateSettingsAfterRename->isChecked());
    m_rqsetSettings.setValue("ReSortFileListAfterRename", m_pqcbReSortFileListAfterRename->isChecked());
    m_rqsetSettings.endGroup();

    m_rqsetSettings.beginGroup("Interface");
    m_rqsetSettings.setValue("ShowSideBar", m_pqcbShowSideBar->isChecked());
    m_rqsetSettings.endGroup();

    m_pmwMainWindow->GetFileListUI()->GetInvCharSub().SavePreferencesChanges(m_pqtwInvalidCharacterTable);

    m_rqsetSettings.sync();
    ProcessChanges();

    return true;
}


void IDlgPreferences::ProcessChanges()
{
    m_pipgGeneralPreferences->ProcessChanges();

    IUIFileList* & rpuifmFileList = m_pmwMainWindow->GetFileListUI();
    rpuifmFileList->SetAutoRefreshEnabled(m_pqcbAutoRefreshEnabled->isChecked());
    rpuifmFileList->SetOpenFileWhenDblClicked(m_pqcbOpenFileWhenDblClicked->isChecked());

    if (m_pqcbNameChangeColourText->isChecked()   != rpuifmFileList->GetNameChangeColourText()   ||
        m_pqcbNameChangeHighlightRow->isChecked() != rpuifmFileList->GetNameChangeHighlightRow() ||
        m_qcolNameChangeTextColour                != rpuifmFileList->GetNameChangeTextColour()   ||
        m_qcolNameChangeHighlightColour           != rpuifmFileList->GetNameChangeHighlightColour())
    {
        rpuifmFileList->UpdateChangeHighlightSettings(m_pqcbNameChangeColourText->isChecked(), m_qcolNameChangeTextColour,
                                                       m_pqcbNameChangeHighlightRow->isChecked(), m_qcolNameChangeHighlightColour);
    }

    bool bUseAlternativeFont = m_pqcbUseAlternativeFont->isChecked();
    rpuifmFileList->SetUseAlternativeFont(bUseAlternativeFont);
    if (bUseAlternativeFont == false)
        rpuifmFileList->RestoreDefaultFonts();
    else
        rpuifmFileList->SetTableFont(m_qfntSelectedFont);
    rpuifmFileList->SetUserDefinedRowHeight(m_pqcbUseCustomLineSpacing->isChecked(), m_pqleCustomLineSpacingPxl->text().toInt());
    rpuifmFileList->SetRowHeightAndIconSize();

    rpuifmFileList->SetShowConfirmBeforeRename(m_pqcbShowConfirmBeforeRename->isChecked());
    rpuifmFileList->SetDeactivateSettingsAfterRename(m_pqcbDeactivateSettingsAfterRename->isChecked());
    rpuifmFileList->SetReSortFilesAfterRename(m_pqcbReSortFileListAfterRename->isChecked());

    m_pmwMainWindow->SetSideBarEnabled(m_pqcbShowSideBar->isChecked());

    if (m_pmwMainWindow->GetFileListUI()->GetInvCharSub().ChangesMade())
        rpuifmFileList->ReReadMetaTags();
}


bool IDlgPreferences::ValidateStartDirectory(const QString & krqstrPath)
{
    if (m_pqrbStartDirectoryCustom->isChecked() == false)
        return true;

    QFileInfo qfinfFile(krqstrPath);
    if (qfinfFile.exists() == false)
    {
        QMessageBox::warning(this, tr("Custom Start Directory Invalid"), tr("The entered start directory is not valid.\nPlease specify a valid directory."), QMessageBox::Ok);
        return false;
    }

    if (qfinfFile.isFile())
        m_pqleStartDirectory->setText(QDir::toNativeSeparators(qfinfFile.path()));

    return true;
}


void IDlgPreferences::accept()
{
    if (ValidateStartDirectory(m_pqleStartDirectory->text()) == false)
        return;

    if (SaveSettings() == false)
        return;

    QDialog::accept();
}


void IDlgPreferences::done(const int kiResult)
{
    m_iStartPage = m_pqlwPageList->currentRow();
    QDialog::done(kiResult);
}


void IDlgPreferences::SetCustomStartDirectory()
{
    QFileDialog qfdFileDlg(this);
    qfdFileDlg.setFileMode(QFileDialog::Directory);
    qfdFileDlg.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    qfdFileDlg.setWindowTitle(tr("Specify Start Directory"));

    if (m_pqleStartDirectory->text().isEmpty() == false && QFileInfo::exists(QDir::fromNativeSeparators(m_pqleStartDirectory->text())))
    {
        qfdFileDlg.setDirectory(QDir::fromNativeSeparators(m_pqleStartDirectory->text()));
    }
    else
    {
        #ifdef Q_OS_WIN
        qfdFileDlg.setDirectoryUrl(IComUtilityFuncs::GetMyComputerURL());
        #else
        qfdFileDlg.setDirectory(QDir::homePath());
        #endif
    }

    if (qfdFileDlg.exec())
    {
        m_pqleStartDirectory->setText(QDir::toNativeSeparators(qfdFileDlg.selectedFiles().at(0)));
        m_pqrbStartDirectoryCustom->setChecked(true);
    }
}


void IDlgPreferences::SetColourChangesColour()
{
    QColor qcolColour = QColorDialog::getColor(m_qcolNameChangeTextColour, this);
    if (qcolColour.isValid())
    {
        m_qcolNameChangeTextColour = qcolColour;
        m_pqpbNameChangeTextColour->setStyleSheet(QString("QPushButton { background-color : %1;}").arg(qcolColour.name()));
    }
}


void IDlgPreferences::SetHighlightChangesColour()
{
    QColor qcolColour = QColorDialog::getColor(m_qcolNameChangeHighlightColour, this);
    if (qcolColour.isValid())
    {
        m_qcolNameChangeHighlightColour = qcolColour;
        m_pqpbNameChangeHighlightColour->setStyleSheet(QString("QPushButton { background-color : %1;}").arg(qcolColour.name()));
    }
}


void IDlgPreferences::SetFileListFont()
{  
    bool bOK;
    QFont qfntFont = QFontDialog::getFont(&bOK, m_qfntSelectedFont, this);

    if (bOK)
        SetCurrentFont(qfntFont);
}


void IDlgPreferences::PopulateCharSubTableDefault()
{
    m_pmwMainWindow->GetFileListUI()->GetInvCharSub().PopulateTableDefaultReplacements(m_pqtwInvalidCharacterTable);
}


void IDlgPreferences::PopulateCharSubTableDefaultWindows()
{
    m_pmwMainWindow->GetFileListUI()->GetInvCharSub().PopulateTableDefaultReplacementsWindows(m_pqtwInvalidCharacterTable);
}


void IDlgPreferences::CharSubTableCellChanged()
{
    QLineEdit* pqleEditedItem = qobject_cast<QLineEdit*>(sender());
    const int kiRow = pqleEditedItem->property("Row").toInt();

    if (m_pqtwInvalidCharacterTable->item(kiRow, 0)->text() == pqleEditedItem->text())
        pqleEditedItem->setStyleSheet(QString("color: %1").arg(QApplication::palette().text().color().name()));
    else
        pqleEditedItem->setStyleSheet(QString("color: %1").arg(m_pmwMainWindow->GetFileListUI()->GetNameChangeTextColour().name()));
}


bool IDlgPreferences::StartDirectoryRootDefaultVal()
{
    #ifdef Q_OS_WIN
    return true;
    #else
    return false;
    #endif
}


bool IDlgPreferences::StartDirectoryHomeDefaultVal()
{
    #ifdef Q_OS_WIN
    return false;
    #else
    return true;
    #endif
}

