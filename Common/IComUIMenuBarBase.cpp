#include <QtWidgets>
#include <QAction>
#include "IComUIMenuBarBase.h"
#include "IComDlgHelpAbout.h"
#include "IComDlgCredits.h"
#include "IUIMainWindow.h"
#include "IDlgPreferences.h"


IComUIMenuBarBase::IComUIMenuBarBase(IUIMainWindow* pmwMainWindow) : QObject(pmwMainWindow)
{
    m_pmwMainWindow = pmwMainWindow;
    m_pqactCredits = nullptr;
}


void IComUIMenuBarBase::CreateActions()
{
    m_pqactPreferences = new QAction(tr("&Preferences..."), m_pmwMainWindow);
    m_pqactPreferences->setIcon(QIcon(":/Resources/Settings.png"));
    m_pqactPreferences->setShortcut(QKeySequence::Preferences);
    m_pqactPreferences->setToolTip(tr("Change application settings"));
    QObject::connect(m_pqactPreferences, SIGNAL(triggered()), this, SLOT(ShowPreferencesDialog()));

    m_pqactExit = new QAction(tr("E&xit"), m_pmwMainWindow);
    m_pqactExit->setShortcut(tr("Ctrl+Q", "Quit/Exit application"));
    QObject::connect(m_pqactExit, SIGNAL(triggered()), m_pmwMainWindow, SLOT(close()));


    // Help menu actions
    m_pqactWebsite = new QAction(tr("&Website"), m_pmwMainWindow);
    m_pqactWebsite->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_MessageBoxInformation));
    //m_pqactWebsite->setShortcut(QKeySequence::HelpContents);
    QObject::connect(m_pqactWebsite, SIGNAL(triggered()), this, SLOT(ShowWebsite()));

    m_pqactForum = new QAction(tr("&Forum"), m_pmwMainWindow);
    m_pqactForum->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_MessageBoxQuestion));
    QObject::connect(m_pqactForum, SIGNAL(triggered()), this, SLOT(ShowForum()));

    m_pqactChangelog = new QAction(tr("&Changelog"), m_pmwMainWindow);
    m_pqactChangelog->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_DialogHelpButton));
    QObject::connect(m_pqactChangelog, SIGNAL(triggered()), this, SLOT(ShowChangelog()));

    if (CreditsAvailable())
    {
        m_pqactCredits = new QAction(tr("C&redits"), m_pmwMainWindow);
        QObject::connect(m_pqactCredits, SIGNAL(triggered()), this, SLOT(ShowCreditsDialog()));
    }

    m_pqactHelpAbout = new QAction(tr("&About"), m_pmwMainWindow);
    QObject::connect(m_pqactHelpAbout, SIGNAL(triggered()), this, SLOT(ShowHelpAboutDialog()));
}


void IComUIMenuBarBase::InitialiseMenuBar()
{
    m_pqmenuHelp = m_pmwMainWindow->menuBar()->addMenu(tr("&Help"));
    m_pqmenuHelp->addAction(m_pqactWebsite);
    m_pqmenuHelp->addAction(m_pqactForum);
    m_pqmenuHelp->addAction(m_pqactChangelog);
    m_pqmenuHelp->addSeparator();
    if (m_pqactCredits != nullptr)
        m_pqmenuHelp->addAction(m_pqactCredits);
    m_pqmenuHelp->addAction(m_pqactHelpAbout);
}


void IComUIMenuBarBase::ShowPreferencesDialog()
{
    new IDlgPreferences(m_pmwMainWindow);
}


void IComUIMenuBarBase::ShowWebsite()
{
    QDesktopServices::openUrl(QUrl(m_pmwMainWindow->GetWebsite()));
}


void IComUIMenuBarBase::ShowForum()
{
    QDesktopServices::openUrl(QUrl(m_pmwMainWindow->GetForumAddress()));
}


void IComUIMenuBarBase::ShowChangelog()
{
    QDesktopServices::openUrl(QUrl(m_pmwMainWindow->GetChangelogAddress()));
}


void IComUIMenuBarBase::ShowCreditsDialog()
{
    new IComDlgCredits(m_pmwMainWindow);
}


void IComUIMenuBarBase::ShowHelpAboutDialog()
{
    new IComDlgHelpAbout(m_pmwMainWindow);
}
