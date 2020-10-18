#ifndef IMKVChaptCueshtTagInfo_h
#define IMKVChaptCueshtTagInfo_h

#include <QString>
#include <QJsonObject>
#include <QTextStream>
class IMKVFileInfo;


class IMKVChaptCueshtTagInfo
{
    Q_DECLARE_TR_FUNCTIONS(IMKVChaptCueshtTagInfo);

private:
    // Number of chapters entries in file
    int                         m_iChapterCount;

    // Number of TRACK entries in cuefile
    int                         m_iCueTrackCount;

    // Number of tag entries
    int                         m_iTagCount;

public:
    IMKVChaptCueshtTagInfo();

    // Resets state of all members ready to read the next file
    void Reset();

    // Updates the file information, sets the passed string to the description of the element and returns the number of chapters
    int GenerateChaptersDescription(IMKVFileInfo & rfinfoFileInfo, QString & rqstrDescription);
    int GenerateCuesheetDescription(IMKVFileInfo & rfinfoFileInfo, QString & rqstrDescription);
    int GenerateTagsDescription(IMKVFileInfo & rfinfoFileInfo, QString & rqstrDescription);

    // Read number of chapters from JSON object, returning true if the file has chapters, or false otherwise
    bool ReadChapterInfo(QJsonObject qjobjFileObject);

    // Read number of chapters from the passed Identify verbose chapter line
    void ReadChapterInfo(const QString & krqstrChapterLine);

    // Processes cuesheet, returning true if cuesheet present
    bool ProcessCuesheet(QTextStream & rqtsrmCuesheetStream);

    // Processes tags file, returning true if tags present
    bool ProcessTags(QTextStream & rqtsrmTagsFileStream);
};

#endif // IMKVChaptCueshtTagInfo_h
