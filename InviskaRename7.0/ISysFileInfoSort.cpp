#include "ISysFileInfoSort.h"
#include "IUIFileList.h"


ISysFileInfoSort::ISysFileInfoSort(IUIFileList* puifmFileList)
{
    m_puifmFileList = puifmFileList;
    m_qcolCollator.setNumericMode(true);
}


QFileInfoList ISysFileInfoSort::GetSortedFileList()
{
    QFileInfoList qfilDirList = m_puifmFileList->GetDirectoryFileList(QDir::Dirs);
    std::sort(qfilDirList.begin(), qfilDirList.end(), *this);

    QFileInfoList qfilFileList = m_puifmFileList->GetDirectoryFileList(QDir::Files);
    std::sort(qfilFileList.begin(), qfilFileList.end(), *this);

    qfilDirList.append(qfilFileList);
    return qfilDirList;
}


bool ISysFileInfoSort::ComesAfter(const QString & krqstrNewFile, const QString & krqstrRowFile) const
{
    return m_qcolCollator.compare(krqstrNewFile, krqstrRowFile) > 0;
}


bool ISysFileInfoSort::operator()(const QFileInfo & krqfiFile1, const QFileInfo & krqfiFile2) const
{
    return m_qcolCollator.compare(krqfiFile1.fileName(), krqfiFile2.fileName()) < 0;
}
