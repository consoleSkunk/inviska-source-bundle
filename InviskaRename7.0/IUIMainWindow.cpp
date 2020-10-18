#include <QtWidgets>
#include <QSettings>
#include "IUIMainWindow.h"
#include "IUIFileList.h"
#include "IUIMenuBar.h"
#include "IUIToolBar.h"
#include "IUISideBar.h"
#include "IUIRenameSettings.h"


IUIMainWindow* IUIMainWindow::m_spmwMainWindow = nullptr;

IUIMainWindow::IUIMainWindow(QSettings & rqsetSettings, IComSysSingleInstance & rsnglSingleInstance) : IComUIMainWinBase(rqsetSettings, rsnglSingleInstance)
{
    IUIMainWindow::m_spmwMainWindow = this;

    QWidget* qwCentralWidget = new QWidget(this);
    m_puifmFileList = new IUIFileList(this);
    m_puirsRenameSettingsUI = new IUIRenameSettings(this);

    m_puimbMenuBar = new IUIMenuBar(this);
    m_puitbToolBar = new IUIToolBar(this);
    m_puisbSideBar = new IUISideBar(this);

    QHBoxLayout* pqhblTopLayout = new QHBoxLayout;
    pqhblTopLayout->setContentsMargins(0, 0, 0, 0);
    pqhblTopLayout->setSpacing(0);
    pqhblTopLayout->addWidget(m_puisbSideBar);
    pqhblTopLayout->addWidget(m_puifmFileList);
    m_puisbSideBar->setVisible(rqsetSettings.value("Interface/ShowSideBar", false).toBool());

    QVBoxLayout* pvblMainWinLayout = new QVBoxLayout(qwCentralWidget);
    pvblMainWinLayout->setContentsMargins(0, 0, 0, 0);
    pvblMainWinLayout->addLayout(pqhblTopLayout);
    pvblMainWinLayout->addWidget(m_puirsRenameSettingsUI);
    qwCentralWidget->setLayout(pvblMainWinLayout);
    setCentralWidget(qwCentralWidget);

    m_puirsRenameSettingsUI->LoadRenamesFromQSettings();
    m_puifmFileList->InitAfterCreate();

    QTimer::singleShot(0, this, SLOT(FocusAddressBar()));
}


IUIMainWindow::~IUIMainWindow()
{
    delete m_puitbToolBar;
}


void IUIMainWindow::FocusAddressBar()
{
    m_puitbToolBar->FocusAddressBar();
}


void IUIMainWindow::SetSideBarEnabled(const bool kbEnabled)
{
    if (m_puisbSideBar->isVisible() != kbEnabled)
        m_puisbSideBar->setVisible(kbEnabled);
}


bool IUIMainWindow::SideBarEnabled()
{
    return m_puisbSideBar->isVisible();
}

