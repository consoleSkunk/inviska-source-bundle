#ifndef IDlgOrganiseRenames_h
#define IDlgOrganiseRenames_h

#include <QDialog>
#include <QVector>
class QListWidget;
class QPushButton;
class QDialogButtonBox;
class IUIRenameSettings;
class IRenameSave;


class IDlgOrganiseRenames : public QDialog
{
    Q_OBJECT

private:
    // List of renames
    QListWidget*                m_pqlwRenames;

    // Buttons for organising renames
    QPushButton*                m_qpbMoveUp;
    QPushButton*                m_qpbMoveDown;
    QPushButton*                m_qpbDelete;

    // OK and cancel buttons
    QDialogButtonBox*           m_qdbbOKCancel;

    // List of renames that have been deleted
    QVector<IRenameSave*>       m_qvecpsrenRenamesToDelete;

    // Pointer to rename settings widget so we can call it to save settings
    IUIRenameSettings*          m_puirsRenameSettingsUI;

public:
    IDlgOrganiseRenames(IUIRenameSettings* puirsRenameSettingsUI);

private:
    void PopulateRenamesList();

private slots:
    // Slots for performing button actions
    void MoveUp();
    void MoveDown();
    void Delete();

public slots:
    // Override accept() function to process settings when Save is pressed
    void accept();
};

#endif // IDlgOrganiseRenames_h
