#ifndef ISysFileInfoSort_h
#define ISysFileInfoSort_h

#include <QCollator>
#include <QFileInfoList>
class IUIFileList;


class ISysFileInfoSort
{
private:
    // QColator for comparing and sorting filenames
    QCollator           m_qcolCollator;

    // Pointer to file list for getting directory listings
    IUIFileList*        m_puifmFileList;

public:
    ISysFileInfoSort(IUIFileList* puifmFileList);

    // Returns a sorted file list with directories first and natural number sorting
    QFileInfoList GetSortedFileList();

    // Returns true if the new file comes after the row file
    bool ComesAfter(const QString & krqstrNewFile, const QString & krqstrRowFile) const;

    // Operator for std::sort
    bool operator()(const QFileInfo & krqfiFile1, const QFileInfo & krqfiFile2) const;
};

#endif // ISysFileInfoSort_h
