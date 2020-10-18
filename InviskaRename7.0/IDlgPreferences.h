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

    // Currently selected colours for text and row highlights
    QColor                      m_qcolNameChangeTextColour;
    QColor                      m_qcolNameChangeHighlightColour;

    // If the selected font is the same as the MS Shell Dialog 2 logical font it returns MS Shell Dialog 2
    // This Stores the currently selected font so we always have the correct font
    QFont                       m_qfntSelectedFont;

    // Stores the start page so it can be restored the next time the dialog is loaded
    static int                  m_iStartPage;

public:
    IDlgPreferences(IUIMainWindow* pmwMainWindow, const int kiStartPage = -1);

private:
    // Configures settings for invalid character substitution table and populates table
    void InitCharSubTable();

    // Reads current settings and sets the state of the controls to match
    void SetWidgetStates();

    // Sets the font button font and texts and stores font in m_qfntSelectedFont
    void SetCurrentFont(const QFont & krqfntFont);

    // Save settings, returning true if settings are valid and have been saved, or false if the dialog should be kept open
    bool SaveSettings();

    // process any changes that need applying
    void ProcessChanges();

    // Confirms the specified start directory exists, returning true if it does
    bool ValidateStartDirectory(const QString & krqstrPath);

public slots:
    // Override accept() function to process settings when OK is pressed
    void accept();

    // Override done so currently selected row can be saved
    void done(const int kiResult);

private slots:
    // Shows directory select dialog to specify custom start directory
    void SetCustomStartDirectory();

    // Shows colour select dialog to set the colour for text and row highlights
    void SetColourChangesColour();
    void SetHighlightChangesColour();

    // Shows font select dialog so user can select font used in file list
    void SetFileListFont();

    // Populates character substitution table with default values
    void PopulateCharSubTableDefault();
    void PopulateCharSubTableDefaultWindows();

    // Called when contents of character substitution table is changed so colouring can be performed
    void CharSubTableCellChanged();

public:
    // Returnds default value for start directory settings
    static bool StartDirectoryRootDefaultVal();
    static bool StartDirectoryHomeDefaultVal();
};

#endif // IDlgPreferences_h
