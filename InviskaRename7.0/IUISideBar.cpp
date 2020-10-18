#include <QtWidgets>
#include <QAction>
#include "IUISideBar.h"
#include "IUIMainWindow.h"
#include "IUIMenuBar.h"


IUISideBar::IUISideBar(IUIMainWindow* pmwMainWindow) :  QToolBar(pmwMainWindow)
{  
    setOrientation(Qt::Vertical);
    setMovable(false);

    QWidget* pqwidSpacerTop = new QWidget(this);
    QWidget* pqwidSpacerBottom = new QWidget(this);
    pqwidSpacerTop->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    pqwidSpacerBottom->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    addWidget(pqwidSpacerTop);
    pmwMainWindow->GetMenuBar()->AddSideBarActions(this);
    addWidget(pqwidSpacerBottom);
}
