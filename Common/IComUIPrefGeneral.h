#ifndef IComUIPrefGeneral_h
#define IComUIPrefGeneral_h

#include <QWidget>
#include <QSettings>
#include "ui_UIComPrefGeneral.h"
class IUIMainWindow;
class IDlgPreferences;


class IComUIPrefGeneral : public QWidget, public Ui::UIComPrefGeneral
{
    Q_OBJECT

private:
    // Main window
    IUIMainWindow*              m_pmwMainWindow;

    // Application settings
    QSettings &                 m_rqsetSettings;

    // Validator for QLineEdits that should accept integars only
    QIntValidator*              m_pivalIntOnlyValidator;

    // Stores window position settings, so if they are changed we can reposition the window
    bool                        m_bWinPosUseSpecified;
    QRect                       m_qrctWinRect;
    int                         m_iWinPosY;
    int                         m_iWinWidth;
    int                         m_iWinHeight;

public:
    IComUIPrefGeneral(IDlgPreferences* pdprefPreferencesDlg, IUIMainWindow* pmwMainWindow);

    // Reads current settings and sets the state of the controls to match
    void SetWidgetStates();

    // Save settings and process any changes that need applying
    bool SaveSettings();
    void ProcessChanges();

private:
    // Confirms the specified window position is onscreen.  If so moves it and returns true, and if not displays a message and returns false
    bool ValidateWinPositionAndMove();
};

#endif // IComUIPrefGeneral_h
