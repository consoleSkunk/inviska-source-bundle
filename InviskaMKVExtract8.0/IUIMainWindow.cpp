#include <QtWidgets>
#include "IUIMainWindow.h"
#include "IUIExtract.h"
#include "IUIMenuBar.h"


IUIMainWindow::IUIMainWindow(QSettings & rqsetSettings, IComSysSingleInstance & rsnglSingleInstance) : IComUIMainWinBase(rqsetSettings, rsnglSingleInstance)
{
    m_puiextExtractUI = new IUIExtract(this);
    setCentralWidget(m_puiextExtractUI);

    m_puimbMenuBar = new IUIMenuBar(this);
}



IUIMainWindow::~IUIMainWindow()
{

}


void IUIMainWindow::ProcessCommandLineParameters()
{
    m_puiextExtractUI->ProcessCommandLineParameters();
}
