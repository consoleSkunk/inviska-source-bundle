#include <QtWidgets>
#include <QAction>
#include "IUIToolBar.h"
#include "IUIMainWindow.h"
#include "IUIFileList.h"
#include "IUIMenuBar.h"
#include "IComQLineEdit.h"


IUIToolBar::IUIToolBar(IUIMainWindow* pmwMainWindow)
{  
    m_pqtpNavigationToolBar = pmwMainWindow->addToolBar(tr("&Navigation"));
    m_pqtpNavigationToolBar->setMovable(false);
    m_pqtpNavigationToolBar->setStyleSheet("QToolBar { border : 0; }");

    m_pqleAddressBox = new IComQLineEdit(pmwMainWindow, true);
    m_pqleAddressBox->setStyleSheet("QLineEdit { margin-right : 3px; }");
    QObject::connect(m_pqleAddressBox, SIGNAL(returnPressed()), pmwMainWindow->GetFileListUI(), SLOT(SetDirectoryFromAddressBar()));

    IUIMenuBar* & rpuimbMenuBar = pmwMainWindow->GetMenuBar();
    rpuimbMenuBar->AddNavigationActions(m_pqtpNavigationToolBar);
    m_pqtpNavigationToolBar->addWidget(m_pqleAddressBox);
    rpuimbMenuBar->AddPreferencesActions(m_pqtpNavigationToolBar);
}


QString IUIToolBar::GetAdddressBarText()
{
    return m_pqleAddressBox->text();
}


void IUIToolBar::AddressBarSelectAll()
{
    m_pqleAddressBox->selectAll();
}


void IUIToolBar::SetAddressBarText(const QString & krstrPath)
{
    m_pqleAddressBox->setText(krstrPath);
}


void IUIToolBar::FocusAddressBar()
{
    m_pqleAddressBox->setFocus();
}

