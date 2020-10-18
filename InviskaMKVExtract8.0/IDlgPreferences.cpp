#include <QtWidgets>
#include "IDlgPreferences.h"
#include "IUIMainWindow.h"
#include "IUIExtract.h"
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

    m_pqcbDetectCuesheetsTags->setChecked(m_pmwMainWindow->GetExtractUI()->DetectCuesheetsTags());
}


bool IDlgPreferences::SaveSettings()
{
    if (m_pipgGeneralPreferences->SaveSettings() == false)
        return false;

    m_rqsetSettings.setValue("MKVToolNix/DetectCuesheetsTags", m_pqcbDetectCuesheetsTags->isChecked());

    m_rqsetSettings.sync();
    ProcessChanges();

    return true;
}


void IDlgPreferences::ProcessChanges()
{
    m_pipgGeneralPreferences->ProcessChanges();

    if (m_bMKVToolNixPathChanged)
    {
        IComSysMKVToolNix* pimkvMKVToolNix = m_pmwMainWindow->GetExtractUI()->GetMKVToolNix();
        pimkvMKVToolNix->SetMKVToolNixPath(m_pqleMKVToolNixDir->text());
        pimkvMKVToolNix->DetermineMKVToolNixVersion();
    }

    m_pmwMainWindow->GetExtractUI()->SetDetectCuesheetsTags(m_pqcbDetectCuesheetsTags->isChecked());
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
    IComSysMKVToolNix* pimkvMKVToolNix = m_pmwMainWindow->GetExtractUI()->GetMKVToolNix();
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
