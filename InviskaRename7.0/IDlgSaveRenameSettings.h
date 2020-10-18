#ifndef IDlgSaveRenameSettings_h
#define IDlgSaveRenameSettings_h

#include <QDialog>
class QLineEdit;
class QDialogButtonBox;
class IUIRenameSettings;


class IDlgSaveRenameSettings : public QDialog
{
    Q_OBJECT

private:
    // Line edit to enter name
    QLineEdit*              m_pqleName;

    // Cancel and Save buttonbox
    QDialogButtonBox*       m_pqdbbCancelSave;

    // Pointer to rename settings widget so we can call it to save settings
    IUIRenameSettings*      m_puirsRenameSettingsUI;

public:
    IDlgSaveRenameSettings(IUIRenameSettings* puirsRenameSettingsUI);

private slots:
    // Makes sure save button is only avalable when the Name line edit contains text
    void EnableDisableSaveButton();

public slots:
    // Override accept() function to process settings when Save is pressed
    void accept();
};

#endif // IDlgSaveRenameSettings_h
