#ifndef IComWdgtMetaMusic_h
#define IComWdgtMetaMusic_h

#include <QTableWidget>
#include <QPushButton>
#include "IComQLineEdit.h"
class QLineEditTag;
class IComMetaMusic;
class QLineEditTag;


class IComWdgtMetaMusic : public QTableWidget
{
    Q_OBJECT

private:
    // Path of file for which information is displayed
    QString                     m_qstrFilePath;

    // Line edits to be used for editable values
    QLineEditTag*               m_pqletTitle;
    QLineEditTag*               m_pqletArtist;
    QLineEditTag*               m_pqletAlbum;
    QLineEditTag*               m_pqletTrack;
    QLineEditTag*               m_pqletYear;
    QLineEditTag*               m_pqletGenre;
    QLineEditTag*               m_pqletComment;

    // Applies changes made to the tags
    QPushButton*                m_pqpbApplyChanges;

    // Validator for year and track number
    QIntValidator*              m_pivalIntOnlyValidator;

    // Array of line edits which define tab order
    static const int            m_kiNumLineEdits = 7;
    QLineEditTag*               m_rgpqletLineEdits[m_kiNumLineEdits];

    // Roles for storing data in QLineEdits
    enum                        LineEditDataRoles {ValueChanged = Qt::UserRole};

public:
    // Creates Exif widget only if Exif data is in the passed file, otherwise returns nullptr
    static IComWdgtMetaMusic* CreateWidget(const QString & krqstrFilePath, QWidget* pqwParent = nullptr);

private:
    IComWdgtMetaMusic(const QString & krqstrFilePath, IComMetaMusic & rmmuMusicMeta, QWidget* pqwParent);

    // Populates table with music meta tag information
    void PopulateTable(IComMetaMusic & rmmuMusicMeta);

    // Helper functions for populating table
    void PopulateTableAddDivision(const QString & krqstrTitle);
    void PopulateTableAddItem(const QString & krqstrTag, const QString & krqstrValue);
    void PopulateTableAddItem(const QString & krqstrTag, const QString & krqstrValue, QLineEditTag* & rpqletLineEdit, bool bIntOnly);
    void PopulateTableAddApplyButton();

    // Disables all editing in line edits (for when something has gone wrong)
    void DisableEditing();

protected:
    // Overrides tab behavior to instead tab through QLineEditTags rather than through all cells
    bool focusNextPrevChild(bool bNext);

private slots:
    // Flags value as modified and enables the apply button allowing tag changes to be saved
    void TagValueModified();

    // Saves updated tags to file
    void SaveUpdatedTags();

private:
    // Save helper functions for handling situations where save was unsuccessful
    bool SaveFileExists();
    bool SaveTagDataPresent(IComMetaMusic & rmmuMusicMeta);
    bool SaveRetrySave(IComMetaMusic & rmmuMusicMeta);
    void SaveCriticalError();

    // Reads and updates modified tag values
    void ReadModifiedValues(IComMetaMusic & rmmuMusicMeta);

    // Clears the modified flag in all QLineEdits
    void ClearModifiedFlags();

public:
    // Creates dialog to display tag inforamation if present, or just returns without doing anything if no tags present
    static void DisplayTags(const QString & krqstrFilePath);
};


class QLineEditTag : public IComQLineEdit
{
    Q_OBJECT

public:
    // Indicates if the text has been modified from initial value
    bool            m_bModified;

public:
    QLineEditTag(QWidget* pqwParent = nullptr) : IComQLineEdit(pqwParent, true) {}
};


#endif // IComWdgtMetaMusic_h

