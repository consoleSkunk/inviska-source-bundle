#ifndef IUISideBar_h
#define IUISideBar_h

#include <QToolBar>
class IUIMainWindow;


class IUISideBar : public QToolBar
{
public:
    IUISideBar(IUIMainWindow* pmwMainWindow);
};

#endif // IUISideBar_h
