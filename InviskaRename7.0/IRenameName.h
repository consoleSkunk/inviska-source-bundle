#ifndef IRenameName_h
#define IRenameName_h

#include <QString>
#include <QList>


struct ITagInfo
{
    // Index of the opening and closing bracket
    int                 m_iStartIndex;
    int                 m_iEndIndex;

    // ID of the tag from IMetaTagLookup::TagID enum
    int                 m_iTagID;
};


class IRenameName
{
public:
    bool                m_bReplaceName;
    QString             m_qstrReplaceName;
    QList<ITagInfo>     m_qlstReplaceNameTags;

    bool                m_bReplaceTheText;
    QString             m_qstrReplaceTheText;
    QString             m_qstrReplaceTheTextWith;
    QList<ITagInfo>     m_qlstReplaceTheTextWithTags;

    bool                m_bInsertTheText;
    QString             m_qstrInsertTheText;
    int                 m_iInsertTheTextAtPos;
    QList<ITagInfo>     m_qlstInsertTheTextTags;

    bool                m_bInsertAtStart;
    QString             m_qstrInsertAtStart;
    QList<ITagInfo>     m_qlstInsertAtStartTags;

    bool                m_bInsertAtEnd;
    QString             m_qstrInsertAtEnd;
    QList<ITagInfo>     m_qlstInsertAtEndTags;

    bool                m_bCropAtPos;
    int                 m_iCropAtPos;
    int                 m_iCropAtPosNextNChar;

    bool                m_bLeftCropNChar;
    int                 m_iLeftCropNChar;

    bool                m_bRightCropNChar;
    int                 m_iRightCropNChar;

    int                 m_iChangeCase;

    enum                ChangeCase {CaseNoChange, CaseTitle, CaseSentance, CaseLower, CaseUpper};

public:
    IRenameName();

    // Functions for saving the data to, and writing it from, a QString for strong in the settings file
    QString OutputToString() const;
    void    ReadFromString(const QString & krqstrInput);
};

#endif // IRenameName_h
