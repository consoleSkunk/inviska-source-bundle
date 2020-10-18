#include <QtWidgets>
#include <QAction>
#include "IUIMenuBar.h"
#include "IUIMainWindow.h"
#include "IUIInfoDisplay.h"


IUIMenuBar::IUIMenuBar(IUIMainWindow* pmwMainWindow) : IComUIMenuBarBase(pmwMainWindow)
{
    CreateActions();
    InitialiseMenuBar();
}


void IUIMenuBar::CreateActions()
{
    IUIInfoDisplay* & puiidInfoDisplayUI = m_pmwMainWindow->GetInfoDisplayUI();

    // File menu actions
    m_pqactOpenFiles = new QAction(tr("&Open File..."), m_pmwMainWindow);
    m_pqactOpenFiles->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_DirOpenIcon));
    m_pqactOpenFiles->setShortcut(QKeySequence::Open);
    QObject::connect(m_pqactOpenFiles, SIGNAL(triggered()), puiidInfoDisplayUI, SLOT(OpenFileDialog()));

    m_pqactSaveInfoOuptut = new QAction(tr("&Save MKVInfo Output..."), m_pmwMainWindow);
    m_pqactSaveInfoOuptut->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_DialogSaveButton));
    m_pqactSaveInfoOuptut->setShortcut(QKeySequence::Save);
    m_pqactSaveInfoOuptut->setEnabled(false);
    QObject::connect(m_pqactSaveInfoOuptut, SIGNAL(triggered()), puiidInfoDisplayUI, SLOT(SaveMKVInfoOutput()));

    m_pqactSaveMergeOuptut = new QAction(tr("&Save MKVMerge Output..."), m_pmwMainWindow);
    m_pqactSaveMergeOuptut->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_DialogSaveButton));
    m_pqactSaveMergeOuptut->setShortcut(QKeySequence::Save);
    m_pqactSaveMergeOuptut->setEnabled(false);
    QObject::connect(m_pqactSaveMergeOuptut, SIGNAL(triggered()), puiidInfoDisplayUI, SLOT(SaveMKVMergeOutput()));

    // Scan level actions
    m_pqactScanStandard = new QAction(tr("&Standard"), m_pmwMainWindow);
    m_pqactScanStandard->setCheckable(true);
    m_pqactScanStandard->setChecked(!puiidInfoDisplayUI->GetScanVerbose());
    QObject::connect(m_pqactScanStandard, SIGNAL(triggered()), puiidInfoDisplayUI, SLOT(SetScanStandard()));

    m_pqactScanVerbose = new QAction(tr("&Verbose"), m_pmwMainWindow);
    m_pqactScanVerbose->setCheckable(true);
    m_pqactScanVerbose->setChecked(puiidInfoDisplayUI->GetScanVerbose());
    QObject::connect(m_pqactScanVerbose, SIGNAL(triggered()), puiidInfoDisplayUI, SLOT(SetScanVerbose()));

    m_pqacgScanLevelGroup = new QActionGroup(m_pmwMainWindow);
    m_pqacgScanLevelGroup->addAction(m_pqactScanStandard);
    m_pqacgScanLevelGroup->addAction(m_pqactScanVerbose);

    IComUIMenuBarBase::CreateActions();
}


void IUIMenuBar::InitialiseMenuBar()
{
    m_pqmenuFile = m_pmwMainWindow->menuBar()->addMenu(tr("&File"));
    m_pqmenuFile->addAction(m_pqactOpenFiles);
    m_pqmenuFile->addAction(m_pqactSaveInfoOuptut);
    m_pqmenuFile->addAction(m_pqactSaveMergeOuptut);
    m_pqmenuFile->addAction(m_pqactPreferences);
    m_pqmenuFile->addSeparator();
    m_pqmenuFile->addAction(m_pqactExit);

    m_pqmenuScanLevel = m_pmwMainWindow->menuBar()->addMenu(tr("&Scan Level"));
    m_pqmenuScanLevel->addAction(m_pqactScanStandard);
    m_pqmenuScanLevel->addAction(m_pqactScanVerbose);

    IComUIMenuBarBase::InitialiseMenuBar();
}


void IUIMenuBar::SetSaveEnabled(const bool kbEnabled)
{
    m_pqactSaveInfoOuptut->setEnabled(kbEnabled);
    m_pqactSaveMergeOuptut->setEnabled(kbEnabled);
}


QString IUIMenuBar::GetCreditsDependency()
{
    return tr("This program relies on <a href=\"https://mkvtoolnix.download/\">MKVToolNix</a> written by Moritz Bunkus.<br>");
}

