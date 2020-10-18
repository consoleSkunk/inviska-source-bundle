#include <QtWidgets>
#include "IUIMainWindow.h"
#include "IUIInfoDisplay.h"
#include "IUIMenuBar.h"


IUIMainWindow::IUIMainWindow(QSettings & rqsetSettings, IComSysSingleInstance & rsnglSingleInstance) : IComUIMainWinBase(rqsetSettings, rsnglSingleInstance)
{
    m_puiidInfoDisplayUI = new IUIInfoDisplay(this);
    setCentralWidget(m_puiidInfoDisplayUI);

    m_puimbMenuBar = new IUIMenuBar(this);
}



IUIMainWindow::~IUIMainWindow()
{

}


void IUIMainWindow::ProcessCommandLineParameters()
{
    m_puiidInfoDisplayUI->ProcessCommandLineParameters();
}
