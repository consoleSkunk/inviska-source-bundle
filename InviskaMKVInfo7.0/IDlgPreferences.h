#ifndef IDlgPreferences_h
#define IDlgPreferences_h

#include <QSettings>
#include "ui_UIPreferences.h"
class IComUIPrefGeneral;
class IUIMainWindow;


class IDlgPreferences : public QDialog, public Ui::UIPreferences
{
    Q_OBJECT

private:
    // Main window
    IUIMainWindow*              m_pmwMainWindow;

    // Application settings
    QSettings &                 m_rqsetSettings;

    // Widget that stores preferences used in all Inviska applications
    IComUIPrefGeneral*          m_pipgGeneralPreferences;

    // Stores the start page so it can be restored the next time the dialog is loaded
    static int                  m_iStartPage;

    // Stores the original MKVToolNix path so we can see if it has changed
    QString                     m_qstrMKVToolNixPathOrig;

    // When accept() is called the path is compared and this value is set
    bool                        m_bMKVToolNixPathChanged;

public:
    // Defines the page the MKVToolNix settings are on
    enum                        Pags {MKVToolNixPage = 1};

public:
    IDlgPreferences(IUIMainWindow* pmwMainWindow, const int kiStartPage = -1);

private:
    // Reads current settings and sets the state of the controls to match
    void SetWidgetStates();

    // Save settings, returning true if settings are valid and have been saved, or false if the dialog should be kept open
    bool SaveSettings();

    // process any changes that need applying
    void ProcessChanges();

    // Confirms the specified MKVToolNix path is valid, returning true if it is
    bool ValidateMKVToolNixPath(const QString & krqstrPath);

private slots:
    // Shows open file dialog to specify MKVToolNix directory
    void SetMKVToolNixPath();

public slots:
    // Override accept() function to process settings when OK is pressed
    void accept();

    // Override done so currently selected row can be saved
    void done(const int kiResult);
};

#endif // IDlgPreferences_h
