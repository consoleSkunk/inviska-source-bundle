#ifndef IUIRenameSettings_h
#define IUIRenameSettings_h

#include <QWidget>
#include "ui_UIRenameSettings.h"
#include "IRenameName.h"
#include "IRenameNumber.h"
#include "IMetaTagLookup.h"
class QListWidget;
class IUIMainWindow;
class IUIFileList;
class IRenameSave;


class IUIRenameSettings : public QWidget, public Ui::UIRenameSettings
{
    Q_OBJECT

private:
    // Pointer to main window so menu bar can be updated with saved renames
    IUIMainWindow*              m_pmwMainWindow;

    // Pointer to file list UI so we can alert it to update name previews when settings change
    IUIFileList* &              m_rpuifmFileList;

    // For looking up values associated with meta tags and replacing those tags in name strings
    IMetaTagLookup &            m_rmtlMetaTagLookup;

    // Validator for QLineEdits that should accept integars only
    QIntValidator*              m_pivalIntOnlyValidator;

    // Validator for QLineEdits that excludes invalid file name characters
    QRegExpValidator*           m_pqrevInvalidCharValidator;

    // Settings to use for renaming files read
    int                         m_iRenameElements;
    QStringList                 m_qstrlRenameExtensions;
    IRenameName                 m_rnamNameSettings;
    IRenameName                 m_rnamExtenSettings;
    IRenameNumber               m_rnumNumberingSettings;

    // Maps insert tag QActions onto the String to insert into the settings box
    QHash<QAction*, QString>    m_qhashActionLookupFiAt;
    QHash<QAction*, QString>    m_qhashActionLookupMusic;
    QHash<QAction*, QString>    m_qhashActionLookupExif;

    // So we can insert tags from the menu bar, this stores the last active line edit and the cursor position
    QLineEdit*                  m_pqleLastActiveLineEdit;

    // After a rename all rename settings are disabled.
    // When the settings change the connections cause GeneratePreviewName/Extension() to be repeteadly called to update the name.
    // This flag indicates that we're clearing the settings so those functions won't repeatedly generate new names unnecessarily.
    bool                        m_bClearingSettings;

    // Vector of pointers to saved rename settings
    QVector<IRenameSave*>       m_qvecpsrenSavedRenames;

public:
    // Values for m_iRenameElements which indicate which files/folders should be renamed
    enum                        RenameElements {RenameFilesOnly, RenameFoldersOnly, RenameFilesAndFolders, RenameSelectedItems, RenameFilesWithExtension};

public:
    IUIRenameSettings(IUIMainWindow* pmwMainWindow);
    ~IUIRenameSettings();

private:
    // Set up regular expression validators for QLineEdits
    void SetNumbersOnlyValidators();
    void SetInvalidCharactersValidators();

    // Set up connectiosn to signal when the name/extension/numbering settings change so we can update the name preivew
    void CreateNameSettingsConnections();
    void CreateExtensionSettingsConnections();
    void CreateNumberingSettingsConnections();
    void CreateGlobalRenameSettingsConnections();

    // Disables all rename settings, but doesn't clear boxes
    void DisableAllSettings();

public slots:
    // Disables all rename settings, and clears all boxes
    void ClearAll();

private slots:
    // Stores last active line edit for insertion of tags
    void StoreLastActiveLineEdit();

    // Clears the last active line pointer, for when the last active line was a line that doesn't support tags
    void ClearLastActiveLineEdit();

public:
    // Adds items to Action to TagCode lookup hashes
    void AddActionLookupMusic(QAction* qactAction, const QString & qstrTagCode);
    void AddActionLookupExif(QAction* qactAction, const QString & qstrTagCode);
    void AddActionLookupFiAt(QAction* qactAction, const QString & qstrTagCode);

private slots:
    // Inserts the tag code corresponding the the action that called this slot into the last active QLineEdit
    void InsertTagCodeMusic();
    void InsertTagCodeExif();
    void InsertTagCodeFiAt();

private:
    // Reads the tag codes from the passed string and stores the TagInfo in the passed list
    void ReadTagCodes(const QString & krqstrString, QList<ITagInfo> & rqlstTagList);

private slots:
    // Reads updated settings from UI and calls update on file viewer
    void NameSettingsChangedReplaceName();
    void NameSettingsChangedReplaceTheText();
    void NameSettingsChangedInsertTheText();
    void NameSettingsChangedInsertAtStart();
    void NameSettingsChangedInsertAtEnd();
    void NameSettingsChangedCropAtPos();
    void NameSettingsChangedLeftCrop();
    void NameSettingsChangedRightCrop();
    void NameSettingsChangedChangeCase();

    void ExtensionSettingsChangedReplaceName();
    void ExtensionSettingsChangedReplaceTheText();
    void ExtensionSettingsChangedInsertTheText();
    void ExtensionSettingsChangedInsertAtStart();
    void ExtensionSettingsChangedInsertAtEnd();
    void ExtensionSettingsChangedCropAtPos();
    void ExtensionSettingsChangedLeftCrop();
    void ExtensionSettingsChangedRightCrop();
    void ExtensionSettingsChangedChangeCase();

    void NumberingRadioSettingsChanged(const bool kbEnabled);
    void NumberingSettingsChanged();
    void RenameElementsRadioSettingsChanged(const bool kbEnabled);
    void RenameElementsExtensionChanged();

public:
    // Reads the extensions, converting them to lower case depending on kbCaseSensitive, and storing them
    void ReadRenameExtensions(const bool kbCaseSensitive);

private slots:
    // Displays messagebox to confirm rename/undo action
    void ConfirmRename();
    void ConfirmUndo();

    // Displays dialog prompting user for name and saves settings
    void ShowSaveRenameSettingsDialog();

    // Displays dialog allowing user to reorganise and deleted saved rename settings
    void ShowOrganiseSavedRenamesDialog();

    // Loads the saved settings stored in the sending QAction's data varient
    void LoadSavedRanmeSetting();

public:
    // Saves the current rename settings under the passed name
    void SaveRenameSettings(const QString & krqstrName, IRenameSave* psrsOverwriteSettings = nullptr);

    // Loads renames from QSettings
    void LoadRenamesFromQSettings();

    // Saves all saved renames to QSettings
    void SaveRenamesToQSettings();

    // Returns pointer to saved rename settings with passed name if exists, otherwise returns nullptr
    IRenameSave* GetRenameSettingsFromName(const QString & krqstrName);
    const QVector<IRenameSave*> & GetSavedRenameSettings() {return m_qvecpsrenSavedRenames;}

    // Clears the current rename list and reads the renames from the passed QListWidget
    void ReadRenamesFromListWidget(const QListWidget* kpqlwRenameList);

    // Indicates if auto-numbering is enabled
    bool  Numberingenabled()                            {return !m_pqrbNumberingNoNumber->isChecked();}

    // Accessors
    const IRenameName & GetNameSettings()               {return m_rnamNameSettings;}
    const IRenameName & GetExtenSettings()              {return m_rnamExtenSettings;}
    const IRenameNumber & GetNumberingSettings()        {return m_rnumNumberingSettings;}
    const QStringList & GetRenameExtensions()           {return m_qstrlRenameExtensions;}
    int   RenameElements()                              {return m_iRenameElements;}
    bool  ClearingSettings()                            {return m_bClearingSettings;}
};

#endif // IUIRenameSettings_h
