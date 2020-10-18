#include <QtWidgets>
#include "IDlgPreferences.h"
#include "IUIMainWindow.h"
#include "IUIStripper.h"
#include "IComUIPrefGeneral.h"

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

    resize(sizeHint());
    show();
}


void IDlgPreferences::SetWidgetStates()
{   
    m_pipgGeneralPreferences->SetWidgetStates();

    m_rqsetSettings.beginGroup("StripSettings");
    if (m_rqsetSettings.value("SaveFilesToSubFolder", false).toBool())
        m_pqrbOutputToSubDir->setChecked(true);
    else
        m_pqrbOutputAppendStripped->setChecked(true);

    if (m_rqsetSettings.value("StripCommentsDefaultEnabled", false).toBool())
        m_prbStripCommentsChecked->setChecked(true);
    else
        m_prbStripCommentsUnchecked->setChecked(true);
    m_rqsetSettings.endGroup();
}


bool IDlgPreferences::SaveSettings()
{
    if (m_pipgGeneralPreferences->SaveSettings() == false)
        return false;

    m_rqsetSettings.beginGroup("StripSettings");
    m_rqsetSettings.setValue("SaveFilesToSubFolder", m_pqrbOutputToSubDir->isChecked());
    m_rqsetSettings.setValue("StripCommentsDefaultEnabled", m_prbStripCommentsChecked->isChecked());
    m_rqsetSettings.endGroup();

    m_rqsetSettings.sync();
    ProcessChanges();

    return true;
}


void IDlgPreferences::ProcessChanges()
{
    m_pipgGeneralPreferences->ProcessChanges();

    IUIStripper* & rpuisStripControls = m_pmwMainWindow->GetStripperUI();
    rpuisStripControls->SetStripCommentsCheckbox(m_prbStripCommentsChecked->isChecked());
    rpuisStripControls->SetSaveFilesToSubFolder(m_pqrbOutputToSubDir->isChecked());
}


void IDlgPreferences::accept()
{
    if (SaveSettings() == false)
        return;

    QDialog::accept();
}


void IDlgPreferences::done(const int kiResult)
{
    m_iStartPage = m_pqlwPageList->currentRow();
    QDialog::done(kiResult);
}
