#ifndef IDlgRenameFile_h
#define IDlgRenameFile_h

#include <QDialog>
#include <QDir>
class QLineEdit;
class QDialogButtonBox;
class QRegExpValidator;
class IUIFileList;


class IDlgRenameFile : public QDialog
{
    Q_OBJECT

private:
    // Reference to QDir from iUIFileList
    QDir &                      m_rqdirDirReader;

    // Line edit to enter new name
    QLineEdit*                  m_pqleNewName;

    // Validator that excludes invalid file name characters
    QRegExpValidator*           m_pqrevInvalidCharValidator;

    // Cancel and Rename buttonbox
    QDialogButtonBox*           m_pqdbbCancelRename;

    // Copy of original file name so we can confirm it has changed and enable/disable the Rename button
    QString                     m_qstrOriginalFileName;

    // Padding to use in addition to string length when resizing QLineEdit to fit text
    int                         m_iNamePadding;

public:
    IDlgRenameFile(IUIFileList* puifmFileList, QDir & rqdirDirReader, const QString & krqstrFileName);

private slots:
    // Resizes window to fit name and enables/disables the Rename button depending on whether the name has changed
    void FileNameTextChanged();

public slots:
    // Override accept() function to process settings when Save is pressed
    void accept();
};

#endif // IDlgRenameFile_h
