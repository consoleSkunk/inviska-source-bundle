#ifndef IComWdgtMetaExif_h
#define IComWdgtMetaExif_h

#include <QTableWidget>
#include "IComMetaExif.h"


class IComWdgtMetaExif : public QTableWidget
{
    Q_OBJECT

private:
    // Indicates whether rows where the value is empty should be excluded from the table
    bool                        m_bSkipEmptyRows;

public:
    // Creates Exif widget only if Exif data is in the passed file, otherwise returns nullptr
    static IComWdgtMetaExif* CreateWidget(const QString & krqstrFilePath, QWidget* pqwParent = nullptr);

private:
    IComWdgtMetaExif(IComMetaExif & rmexExifMeta, QWidget* pqwParent);

    // Populates table with Exif information
    void PopulateTable(IComMetaExif & rmexExifMeta);

    // Helper functions for populating table
    void PopulateTableAddDivision(const QString & krqstrTitle);
    void PopulateTableAddItem(IComMetaExif & rmexExifMeta, const ExifIfd kexidID, const ExifTag kextaTag);

public:
    // Creates dialog to display tag inforamation if present, or just returns without doing anything if no tags present
    static void DisplayTags(const QString & krqstrFilePath);
};

#endif // IComWdgtMetaExif_h
