#ifndef IComDlgFileProperties_h
#define IComDlgFileProperties_h

#include <QDialog>
#include <QFileInfo>

class QWidget;
class QTabWidget;
class QDialogButtonBox;
class QFrame;

class IComDlgFileProperties : public QDialog
{
    Q_OBJECT

private:
    // Informationf or the file being displayed
    QFileInfo                       m_qfiFileInfo;

    // Disaplays file properties and tags if present
    QTabWidget*                     m_qtabwTabWidget;

    // OK button
    QDialogButtonBox*               m_pqdbbOK;

    // File path with native separators
    QString                         m_qstrFilePathNat;

public:
    IComDlgFileProperties(const QString & krqstrFilePath);
    IComDlgFileProperties(const QFileInfo & krqfiFileInfo);

private:
    // Called by both versions of the constructor to creat the dialog
    void InitDialog();

    // Creates the file properties widget that is used as the main tab on the QTabWidget
    QWidget* CreateFilePropertiesWdgt();

    // Creates a separator line and returns it for adding to the layout
    QFrame* CreateSeparatorLine();

    // Sets the passed strings to the MIME type and description of the file type
    void GetMIMEType(QString & rqstrMIMEType, QString & rqstrTypeDesc);

    // Returs the size of the file as a string in an appropriate size format (KB, MB, GB etc) and in bytes
    QString GetSizeString();

    // Sets the passed string to represent the file permissions
    void GetFilePermissionsString(char* szPermissions);

    // Adds tabs displaying any tags present in the file
    void AddTagsTabs();
};

#endif // IComDlgFileProperties_h
