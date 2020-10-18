#ifndef IUIMenuBar_h
#define IUIMenuBar_h

#include <QVector>
#include "IComUIMenuBarBase.h"
class QToolBar;
class IUISideBar;
class IUIRenameSettings;


class IUIMenuBar : public IComUIMenuBarBase
{
    Q_OBJECT

private:
    // File Menu
    QAction*            m_pqactOpenDirectory;
    QAction*            m_pqactViewFileMetaTags;
    QAction*            m_pqactCaseSensitiveCompare;

    // Navigate
    QMenu*              m_pqmenuNavigate;
    QAction*            m_pqactNavigateBack;
    QAction*            m_pqactNavigateForward;
    QAction*            m_pqactNavigateUp;
    QAction*            m_pqactNavigateRefresh;
    QAction*            m_pqactNavigateStartDir;
    QAction*            m_pqactShowHiddenFiles;
    QAction*            m_pqactMoveSelectedLinesUp;
    QAction*            m_pqactMoveSelectedLinesDown;

    // Tags
    QMenu*              m_pqmenuTags;

    // Tags Music
    QMenu*              m_pqmenuMusic;  
    QAction*            m_pqactTitle;
    QAction*            m_pqactArtist;
    QAction*            m_pqactAlbum;
    QAction*            m_pqactTrack;
    QAction*            m_pqactYear;
    QAction*            m_pqactGenre;
    QAction*            m_pqactComment;
    QAction*            m_pqactRunTime;
    QAction*            m_pqactChannels;
    QAction*            m_pqactSampleRate;
    QAction*            m_pqactBitRate;

    // Tags Exif
    QMenu*              m_pqmenuExif;
    QAction*            m_pqactCameraMake;
    QAction*            m_pqactCameraModel;
    QAction*            m_pqactFNumber;
    QAction*            m_pqactISOSpeed;
    QAction*            m_pqactExposureTime;
    QAction*            m_pqactExposureTimeDec;
    QAction*            m_pqactFocalLength;
    QAction*            m_pqactProgram;
    QAction*            m_pqactPixelDimX;
    QAction*            m_pqactPixelDimY;
    QAction*            m_pqactDateTime;
    QAction*            m_pqactDate;
    QAction*            m_pqactTime;
    QAction*            m_pqactDateYYYY;
    QAction*            m_pqactDateYY;
    QAction*            m_pqactDateMM;
    QAction*            m_pqactDateDD;
    QAction*            m_pqactTimeHH;
    QAction*            m_pqactTimeMM;
    QAction*            m_pqactTimeSS;
    QAction*            m_pqactTimeSubSec;

    // Tags File Attributes
    QMenu*              m_pqmenuAttributes;
    QAction*            m_pqactFiCreatedDateTime;
    QAction*            m_pqactFiCreatedDate;
    QAction*            m_pqactFiCreatedTime;
    QAction*            m_pqactFiCreatedDateYYYY;
    QAction*            m_pqactFiCreatedDateYY;
    QAction*            m_pqactFiCreatedDateMM;
    QAction*            m_pqactFiCreatedDateDD;
    QAction*            m_pqactFiCreatedTimeHH;
    QAction*            m_pqactFiCreatedTimeMM;
    QAction*            m_pqactFiCreatedTimeSS;
    QAction*            m_pqactFiModifiedDateTime;
    QAction*            m_pqactFiModifiedDate;
    QAction*            m_pqactFiModifiedTime;
    QAction*            m_pqactFiModifiedDateYYYY;
    QAction*            m_pqactFiModifiedDateYY;
    QAction*            m_pqactFiModifiedDateMM;
    QAction*            m_pqactFiModifiedDateDD;
    QAction*            m_pqactFiModifiedTimeHH;
    QAction*            m_pqactFiModifiedTimeMM;
    QAction*            m_pqactFiModifiedTimeSS;

    // Renames
    QMenu*              m_pqmenuRenames;
    QAction*            m_pqactSaveCurrentSettings;
    QAction*            m_pqactOrganiseSavedRenames;
    QAction*            m_pqactClearRenameSettings;
    QVector<QAction*>   m_qvecpqactSavedRenames;

public:
    IUIMenuBar(IUIMainWindow* pmwMainWindow);

private:
    // Creates actions for menus
    void CreateActions();
    void CreateFileActions();
    void CreateNavigateActions();
    void CreateTagsActionsMusic();
    void CreateTagsActionsExif();
    void CreateTagsActionsFileAttributes();
    void CreateRenamesActions();

    // Creates menus and adds actions
    void InitialiseMenuBar();

    // Including the tag in the menu item on MacOS causes a question mark to display instead of the tag
    // We therefore don't want to includ the tag in the menu item on MacOS, so this function returns an empty string on the Mac
    QString PrintTagOS(const QString & krqstrTag);

public:
    // Adds navigation actions to navigation toolbar
    void AddNavigationActions(QToolBar* pqtpNavigationToolBar);
    void AddPreferencesActions(QToolBar* pqtpNavigationToolBar);

    // Adds actions to side bar
    void AddSideBarActions(IUISideBar* puisbSideBar);

    // Enables/disables navigation actions depending on current directory
    void EnableBackAction(const bool kbEnable);
    void EnableForwardAction(const bool kbEnable);
    void EnableUpAction(const bool kbEnable);

    // Returns true if shown hidden files option is checked
    bool ShowHiddenFiles();

    // Returns true if case sensitive compare option is checked
    bool CaseSensitiveCompare();

    // Synces the saved rename actions in the Renames menu to the stored renames
    void SyncSavedRenames(IUIRenameSettings* puirsRenameSettingsUI);

    // Execute the rename action for the rename with the passed name
    void ExecuteRename(const QString & krqstrName);

    // Returns credits for display in credits dialog
    virtual bool CreditsAvailable() {return true;}
    QString GetCreditsDependency();
};

#endif // IUIMenuBar_h
