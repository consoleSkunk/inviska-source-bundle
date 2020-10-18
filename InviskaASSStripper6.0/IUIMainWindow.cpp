#include <QtWidgets>
#include "IUIMainWindow.h"
#include "IUIMenuBar.h"
#include "IUIStripper.h"


IUIMainWindow::IUIMainWindow(QSettings & rqsetSettings, IComSysSingleInstance & rsnglSingleInstance) : IComUIMainWinBase(rqsetSettings, rsnglSingleInstance)
{
    m_puisStripControls = new IUIStripper(this);
    setCentralWidget(m_puisStripControls);

    m_puimbMenuBar = new IUIMenuBar(this);
}



IUIMainWindow::~IUIMainWindow()
{

}
