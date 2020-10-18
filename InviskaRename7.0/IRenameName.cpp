#include <QDataStream>
#include <QStringBuilder>
#include "IRenameName.h"
#include "IRenameSave.h"


IRenameName::IRenameName()
{
    m_bReplaceName          = false;
    m_bReplaceTheText       = false;
    m_bInsertTheText        = false;
    m_bInsertAtStart        = false;
    m_bInsertAtEnd          = false;
    m_bCropAtPos            = false;
    m_bLeftCropNChar        = false;
    m_bRightCropNChar       = false;

    m_iInsertTheTextAtPos   = 0;
    m_iCropAtPos            = 0;
    m_iCropAtPosNextNChar   = 0;
    m_iLeftCropNChar        = 0;
    m_iRightCropNChar       = 0;

    m_iChangeCase           = CaseNoChange;
}


QString IRenameName::OutputToString() const
{
    const int kiVersion = 1;
    QChar qchSeparator = IRenameSave::SaveStringSeparator();

    return
    QString::number(kiVersion)                  % qchSeparator %

    QString::number(m_bReplaceName)             % qchSeparator %
                    m_qstrReplaceName           % qchSeparator %

    QString::number(m_bReplaceTheText)          % qchSeparator %
                    m_qstrReplaceTheText        % qchSeparator %
                    m_qstrReplaceTheTextWith    % qchSeparator %

    QString::number(m_bInsertTheText)           % qchSeparator %
                    m_qstrInsertTheText         % qchSeparator %
    QString::number(m_iInsertTheTextAtPos)      % qchSeparator %

    QString::number(m_bInsertAtStart)           % qchSeparator %
                    m_qstrInsertAtStart         % qchSeparator %

    QString::number(m_bInsertAtEnd)             % qchSeparator %
                    m_qstrInsertAtEnd           % qchSeparator %

    QString::number(m_bCropAtPos)               % qchSeparator %
    QString::number(m_iCropAtPos)               % qchSeparator %
    QString::number(m_iCropAtPosNextNChar)      % qchSeparator %

    QString::number(m_bLeftCropNChar)           % qchSeparator %
    QString::number(m_iLeftCropNChar)           % qchSeparator %

    QString::number(m_bRightCropNChar)          % qchSeparator %
    QString::number(m_iRightCropNChar)          % qchSeparator %

    QString::number(m_iChangeCase);
}


void IRenameName::ReadFromString(const QString & krqstrInput)
{
    QStringList qstrlValues = krqstrInput.split(IRenameSave::SaveStringSeparator());

    int iIndex = 0;
    //int iVersion                = qstrlValues.at(iIndex).toInt();

    m_bReplaceName              = qstrlValues.at(++iIndex).toInt();;
    m_qstrReplaceName           = qstrlValues.at(++iIndex);

    m_bReplaceTheText           = qstrlValues.at(++iIndex).toInt();;
    m_qstrReplaceTheText        = qstrlValues.at(++iIndex);
    m_qstrReplaceTheTextWith    = qstrlValues.at(++iIndex);

    m_bInsertTheText            = qstrlValues.at(++iIndex).toInt();;
    m_qstrInsertTheText         = qstrlValues.at(++iIndex);
    m_iInsertTheTextAtPos       = qstrlValues.at(++iIndex).toInt();;

    m_bInsertAtStart            = qstrlValues.at(++iIndex).toInt();;
    m_qstrInsertAtStart         = qstrlValues.at(++iIndex);

    m_bInsertAtEnd              = qstrlValues.at(++iIndex).toInt();;
    m_qstrInsertAtEnd           = qstrlValues.at(++iIndex);

    m_bCropAtPos                = qstrlValues.at(++iIndex).toInt();;
    m_iCropAtPos                = qstrlValues.at(++iIndex).toInt();;
    m_iCropAtPosNextNChar       = qstrlValues.at(++iIndex).toInt();;

    m_bLeftCropNChar            = qstrlValues.at(++iIndex).toInt();;
    m_iLeftCropNChar            = qstrlValues.at(++iIndex).toInt();;

    m_bRightCropNChar           = qstrlValues.at(++iIndex).toInt();;
    m_iRightCropNChar           = qstrlValues.at(++iIndex).toInt();;

    m_iChangeCase               = qstrlValues.at(++iIndex).toInt();;
}
