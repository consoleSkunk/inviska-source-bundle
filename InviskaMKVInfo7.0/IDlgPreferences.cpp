#include <QtWidgets>
#include "IDlgPreferences.h"
#include "IUIMainWindow.h"
#include "IUIInfoDisplay.h"
#include "IComSysMKVToolNix.h"
#include "IComUIPrefGeneral.h"
#include "IComUtilityFuncs.h"

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

    SetWidgetStates();
    if (kiStartPage != -1)
        m_iStartPage = kiStartPage;
    m_pqlwPageList->setCurrentRow(m_iStartPage);
    m_pqswPageStack->setCurrentIndex(m_iStartPage);

    m_pqpbMKVToolNixLocate->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_DirOpenIcon));
    connect(m_pqpbMKVToolNixLocate, SIGNAL(clicked()), this, SLOT(SetMKVToolNixPath()));

    resize(sizeHint());
    show();
}


void IDlgPreferences::SetWidgetStates()
{   
    m_pipgGeneralPreferences->SetWidgetStates();

    m_qstrMKVToolNixPathOrig = QDir::toNativeSeparators(m_rqsetSettings.value("MKVToolNix/MKVToolNixPath", "").toString());
    #ifdef Q_OS_MACOS
    if (m_qstrMKVToolNixPathOrig.endsWith("/Contents/MacOS"))
        m_qstrMKVToolNixPathOrig.truncate(m_qstrMKVToolNixPathOrig.length() - 15);
    #endif
    m_pqleMKVToolNixDir->setText(m_qstrMKVToolNixPathOrig);
    m_pqleMKVToolNixDir->setCursorPosition(0);

    int iDefaultTab = m_rqsetSettings.value("Application/DefaultTab", IUIInfoDisplay::MKVInfoTree).toInt();
    if (iDefaultTab == IUIInfoDisplay::MKVInfoTree)
        m_pqrbDefaultTabMInfoTree->setChecked(true);
    else if (iDefaultTab == IUIInfoDisplay::MKVInfoText)
        m_pqrbDefaultTabMInfoText->setChecked(true);
    else if (iDefaultTab == IUIInfoDisplay::MKVExtractIdentify)
        m_pqrbDefaultTabMExtractIden->setChecked(true);

    m_pqcbRestoreScanLevel->setChecked(m_rqsetSettings.value("Application/RestoreScanLevel", "false").toBool());
}


bool IDlgPreferences::SaveSettings()
{
    if (m_pipgGeneralPreferences->SaveSettings() == false)
        return false;

    if (m_pqrbDefaultTabMInfoTree->isChecked())
        m_rqsetSettings.setValue("Application/DefaultTab", IUIInfoDisplay::MKVInfoTree);
    else if (m_pqrbDefaultTabMInfoText->isChecked())
        m_rqsetSettings.setValue("Application/DefaultTab", IUIInfoDisplay::MKVInfoText);
    else if (m_pqrbDefaultTabMExtractIden->isChecked())
        m_rqsetSettings.setValue("Application/DefaultTab", IUIInfoDisplay::MKVExtractIdentify);

    m_rqsetSettings.setValue("Application/RestoreScanLevel", m_pqcbRestoreScanLevel->isChecked());

    m_rqsetSettings.sync();
    ProcessChanges();

    return true;
}


void IDlgPreferences::ProcessChanges()
{
    m_pipgGeneralPreferences->ProcessChanges();

    if (m_bMKVToolNixPathChanged)
    {
        IComSysMKVToolNix* pimkvMKVToolNix = m_pmwMainWindow->GetInfoDisplayUI()->GetMKVToolNix();
        pimkvMKVToolNix->SetMKVToolNixPath(m_pqleMKVToolNixDir->text());
        pimkvMKVToolNix->DetermineMKVToolNixVersion();
    }
}


void IDlgPreferences::SetMKVToolNixPath()
{
    QFileDialog qfdFileDlg(this);
    #ifdef Q_OS_MACOS
    qfdFileDlg.setFileMode(QFileDialog::ExistingFile);
    qfdFileDlg.setOptions(QFileDialog::DontResolveSymlinks);
    #else
    qfdFileDlg.setFileMode(QFileDialog::Directory);
    qfdFileDlg.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    #endif

    qfdFileDlg.setWindowTitle(tr("Specify MKVToolNix Directory"));

    #ifdef Q_OS_WIN
    qfdFileDlg.setDirectoryUrl(IComUtilityFuncs::GetMyComputerURL());
    #elif defined Q_OS_MACOS
    qfdFileDlg.setDirectory(QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation).at(1));
    #else
    qfdFileDlg.setDirectory(QDir::rootPath());
    #endif

    if (qfdFileDlg.exec())
    {
        QString qstrDirectory = qfdFileDlg.selectedFiles().at(0);
        if (ValidateMKVToolNixPath(qstrDirectory) == true)
            m_pqleMKVToolNixDir->setText(QDir::toNativeSeparators(qstrDirectory));
    }
}


bool IDlgPreferences::ValidateMKVToolNixPath(const QString & krqstrPath)
{
    IComSysMKVToolNix* pimkvMKVToolNix = m_pmwMainWindow->GetInfoDisplayUI()->GetMKVToolNix();
    if (pimkvMKVToolNix->ValidateMKVToolNixPath(krqstrPath) == false)
    {
        QMessageBox::warning(this, tr("MKVToolNix Location Invalid"), tr("MKVToolNix cannot be found at the specified location.\nPlease specify a valid location for MKVToolNix.\nUse Cancel to exit Preferences without specifying a directory."), QMessageBox::Ok);
        return false;
    }
    return true;
}


void IDlgPreferences::accept()
{
    m_bMKVToolNixPathChanged = (m_pqleMKVToolNixDir->text() != m_qstrMKVToolNixPathOrig);

    if (m_bMKVToolNixPathChanged && ValidateMKVToolNixPath(m_pqleMKVToolNixDir->text()) == false)
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
