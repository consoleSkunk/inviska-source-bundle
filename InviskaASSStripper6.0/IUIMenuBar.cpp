#include <QtWidgets>
#include <QAction>
#include "IUIMenuBar.h"
#include "IUIMainWindow.h"
#include "IUIStripper.h"


IUIMenuBar::IUIMenuBar(IUIMainWindow* pmwMainWindow) : IComUIMenuBarBase(pmwMainWindow)
{
    CreateActions();
    InitialiseMenuBar();
}


void IUIMenuBar::CreateActions()
{
    IUIStripper* & rpuisStripControls = m_pmwMainWindow->GetStripperUI();

    // File menu actions
    m_pqactOpenFiles = new QAction(tr("&Add Files..."), m_pmwMainWindow);
    m_pqactOpenFiles->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_DirOpenIcon));
    m_pqactOpenFiles->setShortcut(QKeySequence::Open);
    QObject::connect(m_pqactOpenFiles, SIGNAL(triggered()), rpuisStripControls, SLOT(OpenFilesDialog()));

    IComUIMenuBarBase::CreateActions();
}


void IUIMenuBar::InitialiseMenuBar()
{
    m_pqmenuFile = m_pmwMainWindow->menuBar()->addMenu(tr("&File"));
    m_pqmenuFile->addAction(m_pqactOpenFiles);
    m_pqmenuFile->addAction(m_pqactPreferences);
    m_pqmenuFile->addSeparator();
    m_pqmenuFile->addAction(m_pqactExit);

    IComUIMenuBarBase::InitialiseMenuBar();
}
