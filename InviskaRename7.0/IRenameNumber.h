#ifndef IRenameNumber_h
#define IRenameNumber_h

#include <QString>


class IRenameNumber
{
public:
    int                 m_iNumberingType;

    int                 m_iNumberingAtPos;

    int                 m_iNumberingStartNum;
    int                 m_iNumberingIncrement;

    bool                m_bNumberingZeroFillAuto;
    int                 m_iNumberingZeroFill;

    enum                Numbering {NumberNoNumbering, NumberAfterName, NumberBeforeName, NumberAtPos};

public:
    IRenameNumber();

    // Functions for saving the data to, and writing it from, a QString for strong in the settings file
    QString OutputToString() const;
    void    ReadFromString(const QString & krqstrInput);
};

#endif // IRenameNumber_h
