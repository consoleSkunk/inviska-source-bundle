#include <QDataStream>
#include <QStringBuilder>
#include "IRenameNumber.h"
#include "IRenameSave.h"


IRenameNumber::IRenameNumber()
{
    m_iNumberingType = NumberNoNumbering;
}


QString IRenameNumber::OutputToString() const
{
    const int kiVersion = 1;
    QChar qchSeparator = IRenameSave::SaveStringSeparator();

    return
    QString::number(kiVersion)                  % qchSeparator %

    QString::number(m_iNumberingType)           % qchSeparator %

    QString::number(m_iNumberingAtPos)          % qchSeparator %

    QString::number(m_iNumberingStartNum)       % qchSeparator %
    QString::number(m_iNumberingIncrement)      % qchSeparator %

    QString::number(m_bNumberingZeroFillAuto)   % qchSeparator %
    QString::number(m_iNumberingZeroFill);
}


void IRenameNumber::ReadFromString(const QString & krqstrInput)
{
    QStringList qstrlValues = krqstrInput.split(IRenameSave::SaveStringSeparator());

    int iIndex = 0;
    //int iVersion              = qstrlValues.at(iIndex).toInt();

    m_iNumberingType            = qstrlValues.at(++iIndex).toInt();

    m_iNumberingAtPos           = qstrlValues.at(++iIndex).toInt();

    m_iNumberingStartNum        = qstrlValues.at(++iIndex).toInt();
    m_iNumberingIncrement       = qstrlValues.at(++iIndex).toInt();

    m_bNumberingZeroFillAuto    = qstrlValues.at(++iIndex).toInt();
    m_iNumberingZeroFill        = qstrlValues.at(++iIndex).toInt();
}
