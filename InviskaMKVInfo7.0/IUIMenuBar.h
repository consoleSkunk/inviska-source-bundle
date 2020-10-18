#ifndef IUIMenuBar_h
#define IUIMenuBar_h

#include "IComUIMenuBarBase.h"
class QActionGroup;


class IUIMenuBar : public IComUIMenuBarBase
{
    Q_OBJECT

private:
    // File Menu
    QAction*                m_pqactOpenFiles;
    QAction*                m_pqactSaveInfoOuptut;
    QAction*                m_pqactSaveMergeOuptut;

    // Scan Level menu
    QMenu*                  m_pqmenuScanLevel;
    QAction*                m_pqactScanStandard;
    QAction*                m_pqactScanVerbose;
    QActionGroup*           m_pqacgScanLevelGroup;

public:
    IUIMenuBar(IUIMainWindow* pmwMainWindow);

private:
    // Creates
    void CreateActions();

    // Creates menus and adds actions
    void InitialiseMenuBar();

public:
    // Enable/Disable Save option
    void SetSaveEnabled(const bool kbEnabled);

    // Returns credits for display in credits dialog
    virtual bool CreditsAvailable() {return true;}
    QString GetCreditsDependency();
};

#endif // IUIMenuBar_h
