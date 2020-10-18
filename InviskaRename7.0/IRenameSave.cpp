#include <QStringBuilder>
#include "IRenameSave.h"
#include "IUIRenameSettings.h"


void IRenameSave::SaveCurrentSettings(const QString & krqstrName, IUIRenameSettings* prsuiRenameSettingsUI)
{
    m_qstrName              = krqstrName;
    m_iRenameElements       = prsuiRenameSettingsUI->RenameElements();
    m_qstrRenameExtensions  = prsuiRenameSettingsUI->m_pqleRenameFilesWithExtension->text();

    SaveNameSettings(prsuiRenameSettingsUI);
    SaveExtensionSettings(prsuiRenameSettingsUI);
    SaveNumberingSettings(prsuiRenameSettingsUI);
}


void IRenameSave::SaveNameSettings(IUIRenameSettings* prsuiRenameSettingsUI)
{
    m_rnamRenameNameSettings.m_bReplaceName             = prsuiRenameSettingsUI->m_pqcbNameReplaceName->isChecked();
    m_rnamRenameNameSettings.m_qstrReplaceName          = prsuiRenameSettingsUI->m_pqleNameReplaceName->text();

    m_rnamRenameNameSettings.m_bReplaceTheText          = prsuiRenameSettingsUI->m_pqcbNameReplaceTheText->isChecked();
    m_rnamRenameNameSettings.m_qstrReplaceTheText       = prsuiRenameSettingsUI->m_pqleNameReplaceTheText->text();
    m_rnamRenameNameSettings.m_qstrReplaceTheTextWith   = prsuiRenameSettingsUI->m_pqleNameReplaceTheTextWith->text();

    m_rnamRenameNameSettings.m_bInsertTheText           = prsuiRenameSettingsUI->m_pqcbNameInsertTheText->isChecked();
    m_rnamRenameNameSettings.m_qstrInsertTheText        = prsuiRenameSettingsUI->m_pqleNameInsertTheText->text();
    m_rnamRenameNameSettings.m_iInsertTheTextAtPos      = ConvertIntForSaving(prsuiRenameSettingsUI->m_pqleNameInsertTheTextAtPos->text());

    m_rnamRenameNameSettings.m_bInsertAtStart           = prsuiRenameSettingsUI->m_pqcbNameInsertAtStart->isChecked();
    m_rnamRenameNameSettings.m_qstrInsertAtStart        = prsuiRenameSettingsUI->m_pqleNameInsertAtStart->text();

    m_rnamRenameNameSettings.m_bInsertAtEnd             = prsuiRenameSettingsUI->m_pqcbNameInsertAtEnd->isChecked();
    m_rnamRenameNameSettings.m_qstrInsertAtEnd          = prsuiRenameSettingsUI->m_pqleNameInsertAtEnd->text();

    m_rnamRenameNameSettings.m_bCropAtPos               = prsuiRenameSettingsUI->m_pqcbNameCropAtPos->isChecked();
    m_rnamRenameNameSettings.m_iCropAtPos               = ConvertIntForSaving(prsuiRenameSettingsUI->m_pqleNameCropAtPos->text());
    m_rnamRenameNameSettings.m_iCropAtPosNextNChar      = ConvertIntForSaving(prsuiRenameSettingsUI->m_pqleNameCropAtPosNextNChar->text());

    m_rnamRenameNameSettings.m_bLeftCropNChar           = prsuiRenameSettingsUI->m_pqcbNameLeftCropNChar->isChecked();
    m_rnamRenameNameSettings.m_iLeftCropNChar           = ConvertIntForSaving(prsuiRenameSettingsUI->m_pqleNameLeftCropNChar->text());

    m_rnamRenameNameSettings.m_bRightCropNChar          = prsuiRenameSettingsUI->m_pqcbNameRightCropNChar->isChecked();
    m_rnamRenameNameSettings.m_iRightCropNChar          = ConvertIntForSaving(prsuiRenameSettingsUI->m_pqleNameRightCropNChar->text());

    m_rnamRenameNameSettings.m_iChangeCase              = prsuiRenameSettingsUI->m_pqcboNameChangeCase->currentIndex();
}


void IRenameSave::SaveExtensionSettings(IUIRenameSettings* prsuiRenameSettingsUI)
{
    m_rnamRenameExtenSettings.m_bReplaceName            = prsuiRenameSettingsUI->m_pqcbExtenReplaceName->isChecked();
    m_rnamRenameExtenSettings.m_qstrReplaceName         = prsuiRenameSettingsUI->m_pqleExtenReplaceName->text();

    m_rnamRenameExtenSettings.m_bReplaceTheText         = prsuiRenameSettingsUI->m_pqcbExtenReplaceTheText->isChecked();
    m_rnamRenameExtenSettings.m_qstrReplaceTheText      = prsuiRenameSettingsUI->m_pqleExtenReplaceTheText->text();
    m_rnamRenameExtenSettings.m_qstrReplaceTheTextWith  = prsuiRenameSettingsUI->m_pqleExtenReplaceTheTextWith->text();

    m_rnamRenameExtenSettings.m_bInsertTheText          = prsuiRenameSettingsUI->m_pqcbExtenInsertTheText->isChecked();
    m_rnamRenameExtenSettings.m_qstrInsertTheText       = prsuiRenameSettingsUI->m_pqleExtenInsertTheText->text();
    m_rnamRenameExtenSettings.m_iInsertTheTextAtPos     = ConvertIntForSaving(prsuiRenameSettingsUI->m_pqleExtenInsertTheTextAtPos->text());

    m_rnamRenameExtenSettings.m_bInsertAtStart          = prsuiRenameSettingsUI->m_pqcbExtenInsertAtStart->isChecked();
    m_rnamRenameExtenSettings.m_qstrInsertAtStart       = prsuiRenameSettingsUI->m_pqleExtenInsertAtStart->text();

    m_rnamRenameExtenSettings.m_bInsertAtEnd            = prsuiRenameSettingsUI->m_pqcbExtenInsertAtEnd->isChecked();
    m_rnamRenameExtenSettings.m_qstrInsertAtEnd         = prsuiRenameSettingsUI->m_pqleExtenInsertAtEnd->text();

    m_rnamRenameExtenSettings.m_bCropAtPos              = prsuiRenameSettingsUI->m_pqcbExtenCropAtPos->isChecked();
    m_rnamRenameExtenSettings.m_iCropAtPos              = ConvertIntForSaving(prsuiRenameSettingsUI->m_pqleExtenCropAtPos->text());
    m_rnamRenameExtenSettings.m_iCropAtPosNextNChar     = ConvertIntForSaving(prsuiRenameSettingsUI->m_pqleExtenCropAtPosNextNChar->text());

    m_rnamRenameExtenSettings.m_bLeftCropNChar          = prsuiRenameSettingsUI->m_pqcbExtenLeftCropNChar->isChecked();
    m_rnamRenameExtenSettings.m_iLeftCropNChar          = ConvertIntForSaving(prsuiRenameSettingsUI->m_pqleExtenLeftCropNChar->text());

    m_rnamRenameExtenSettings.m_bRightCropNChar         = prsuiRenameSettingsUI->m_pqcbExtenRightCropNChar->isChecked();
    m_rnamRenameExtenSettings.m_iRightCropNChar         = ConvertIntForSaving(prsuiRenameSettingsUI->m_pqleExtenRightCropNChar->text());

    m_rnamRenameExtenSettings.m_iChangeCase             = prsuiRenameSettingsUI->m_pqcboExtenChangeCase->currentIndex();
}


void IRenameSave::SaveNumberingSettings(IUIRenameSettings* prsuiRenameSettingsUI)
{
    if (prsuiRenameSettingsUI->m_pqrbNumberingNoNumber->isChecked())
        m_rnumNumberingSettings.m_iNumberingType = IRenameNumber::NumberNoNumbering;
    else if (prsuiRenameSettingsUI->m_pqrbNumberingAfterName->isChecked())
        m_rnumNumberingSettings.m_iNumberingType = IRenameNumber::NumberAfterName;
    else if (prsuiRenameSettingsUI->m_pqrbNumberingBeforeName->isChecked())
        m_rnumNumberingSettings.m_iNumberingType = IRenameNumber::NumberBeforeName;
    else if (prsuiRenameSettingsUI->m_pqrbNumberingAtPos->isChecked())
        m_rnumNumberingSettings.m_iNumberingType = IRenameNumber::NumberAtPos;

    m_rnumNumberingSettings.m_iNumberingAtPos           = ConvertIntForSaving(prsuiRenameSettingsUI->m_pqleNumberingAtPos->text());

    m_rnumNumberingSettings.m_iNumberingStartNum        = prsuiRenameSettingsUI->m_pqleNumberingStartNum->text().toInt();
    m_rnumNumberingSettings.m_iNumberingIncrement       = prsuiRenameSettingsUI->m_pqleNumberingIncrement->text().toInt();

    m_rnumNumberingSettings.m_bNumberingZeroFillAuto    = prsuiRenameSettingsUI->m_pqrbNumberingZeroFillAuto->isChecked();
    m_rnumNumberingSettings.m_iNumberingZeroFill        = prsuiRenameSettingsUI->m_pqleNumberingZeroFill->text().toInt();
}


void IRenameSave::RestoreSettings(IUIRenameSettings* prsuiRenameSettingsUI)
{
    switch (m_iRenameElements)
    {
    case IUIRenameSettings::RenameFilesOnly          : prsuiRenameSettingsUI->m_pqrbRenameFilesOnly->setChecked(true);
                                                       break;
    case IUIRenameSettings::RenameFoldersOnly        : prsuiRenameSettingsUI->m_pqrbRenameFoldersOnly->setChecked(true);
                                                       break;
    case IUIRenameSettings::RenameFilesAndFolders    : prsuiRenameSettingsUI->m_pqrbRenameFilesAndFolders->setChecked(true);
                                                       break;
    case IUIRenameSettings::RenameSelectedItems      : prsuiRenameSettingsUI->m_pqrbRenameSelectedItemsOnly->setChecked(true);
                                                       break;
    case IUIRenameSettings::RenameFilesWithExtension : prsuiRenameSettingsUI->m_pqrbRenameFilesWithExtension->setChecked(true);
                                                       break;
    }

    prsuiRenameSettingsUI->m_pqleRenameFilesWithExtension->setText(m_qstrRenameExtensions);

    RestoreNameSettings(prsuiRenameSettingsUI);
    RestoreExtensionSettings(prsuiRenameSettingsUI);
    RestoreNumberingSettings(prsuiRenameSettingsUI);
}


void IRenameSave::RestoreNameSettings(IUIRenameSettings* prsuiRenameSettingsUI)
{
    prsuiRenameSettingsUI->m_pqcbNameReplaceName->setChecked(m_rnamRenameNameSettings.m_bReplaceName);
    prsuiRenameSettingsUI->m_pqleNameReplaceName->setText(m_rnamRenameNameSettings.m_qstrReplaceName);

    prsuiRenameSettingsUI->m_pqcbNameReplaceTheText->setChecked(m_rnamRenameNameSettings.m_bReplaceTheText);
    prsuiRenameSettingsUI->m_pqleNameReplaceTheText->setText(m_rnamRenameNameSettings.m_qstrReplaceTheText);
    prsuiRenameSettingsUI->m_pqleNameReplaceTheTextWith->setText(m_rnamRenameNameSettings.m_qstrReplaceTheTextWith);

    prsuiRenameSettingsUI->m_pqcbNameInsertTheText->setChecked(m_rnamRenameNameSettings.m_bInsertTheText);
    prsuiRenameSettingsUI->m_pqleNameInsertTheText->setText(m_rnamRenameNameSettings.m_qstrInsertTheText);
    prsuiRenameSettingsUI->m_pqleNameInsertTheTextAtPos->setText(ConvertIntToString(m_rnamRenameNameSettings.m_iInsertTheTextAtPos));

    prsuiRenameSettingsUI->m_pqcbNameInsertAtStart->setChecked(m_rnamRenameNameSettings.m_bInsertAtStart);
    prsuiRenameSettingsUI->m_pqleNameInsertAtStart->setText(m_rnamRenameNameSettings.m_qstrInsertAtStart);

    prsuiRenameSettingsUI->m_pqcbNameInsertAtEnd->setChecked(m_rnamRenameNameSettings.m_bInsertAtEnd);
    prsuiRenameSettingsUI->m_pqleNameInsertAtEnd->setText(m_rnamRenameNameSettings.m_qstrInsertAtEnd);

    prsuiRenameSettingsUI->m_pqcbNameCropAtPos->setChecked(m_rnamRenameNameSettings.m_bCropAtPos);
    prsuiRenameSettingsUI->m_pqleNameCropAtPos->setText(ConvertIntToString(m_rnamRenameNameSettings.m_iCropAtPos));
    prsuiRenameSettingsUI->m_pqleNameCropAtPosNextNChar->setText(ConvertIntToString(m_rnamRenameNameSettings.m_iCropAtPosNextNChar));

    prsuiRenameSettingsUI->m_pqcbNameLeftCropNChar->setChecked(m_rnamRenameNameSettings.m_bLeftCropNChar);
    prsuiRenameSettingsUI->m_pqleNameLeftCropNChar->setText(ConvertIntToString(m_rnamRenameNameSettings.m_iLeftCropNChar));

    prsuiRenameSettingsUI->m_pqcbNameRightCropNChar->setChecked(m_rnamRenameNameSettings.m_bRightCropNChar);
    prsuiRenameSettingsUI->m_pqleNameRightCropNChar->setText(ConvertIntToString(m_rnamRenameNameSettings.m_iRightCropNChar));

    prsuiRenameSettingsUI->m_pqcboNameChangeCase->setCurrentIndex(m_rnamRenameNameSettings.m_iChangeCase);
}


void IRenameSave::RestoreExtensionSettings(IUIRenameSettings* prsuiRenameSettingsUI)
{
    prsuiRenameSettingsUI->m_pqcbExtenReplaceName->setChecked(m_rnamRenameExtenSettings.m_bReplaceName);
    prsuiRenameSettingsUI->m_pqleExtenReplaceName->setText(m_rnamRenameExtenSettings.m_qstrReplaceName);

    prsuiRenameSettingsUI->m_pqcbExtenReplaceTheText->setChecked(m_rnamRenameExtenSettings.m_bReplaceTheText);
    prsuiRenameSettingsUI->m_pqleExtenReplaceTheText->setText(m_rnamRenameExtenSettings.m_qstrReplaceTheText);
    prsuiRenameSettingsUI->m_pqleExtenReplaceTheTextWith->setText(m_rnamRenameExtenSettings.m_qstrReplaceTheTextWith);

    prsuiRenameSettingsUI->m_pqcbExtenInsertTheText->setChecked(m_rnamRenameExtenSettings.m_bInsertTheText);
    prsuiRenameSettingsUI->m_pqleExtenInsertTheText->setText(m_rnamRenameExtenSettings.m_qstrInsertTheText);
    prsuiRenameSettingsUI->m_pqleExtenInsertTheTextAtPos->setText(ConvertIntToString(m_rnamRenameExtenSettings.m_iInsertTheTextAtPos));

    prsuiRenameSettingsUI->m_pqcbExtenInsertAtStart->setChecked(m_rnamRenameExtenSettings.m_bInsertAtStart);
    prsuiRenameSettingsUI->m_pqleExtenInsertAtStart->setText(m_rnamRenameExtenSettings.m_qstrInsertAtStart);

    prsuiRenameSettingsUI->m_pqcbExtenInsertAtEnd->setChecked(m_rnamRenameExtenSettings.m_bInsertAtEnd);
    prsuiRenameSettingsUI->m_pqleExtenInsertAtEnd->setText(m_rnamRenameExtenSettings.m_qstrInsertAtEnd);

    prsuiRenameSettingsUI->m_pqcbExtenCropAtPos->setChecked(m_rnamRenameExtenSettings.m_bCropAtPos);
    prsuiRenameSettingsUI->m_pqleExtenCropAtPos->setText(ConvertIntToString(m_rnamRenameExtenSettings.m_iCropAtPos));
    prsuiRenameSettingsUI->m_pqleExtenCropAtPosNextNChar->setText(ConvertIntToString(m_rnamRenameExtenSettings.m_iCropAtPosNextNChar));

    prsuiRenameSettingsUI->m_pqcbExtenLeftCropNChar->setChecked(m_rnamRenameExtenSettings.m_bLeftCropNChar);
    prsuiRenameSettingsUI->m_pqleExtenLeftCropNChar->setText(ConvertIntToString(m_rnamRenameExtenSettings.m_iLeftCropNChar));

    prsuiRenameSettingsUI->m_pqcbExtenRightCropNChar->setChecked(m_rnamRenameExtenSettings.m_bRightCropNChar );
    prsuiRenameSettingsUI->m_pqleExtenRightCropNChar->setText(ConvertIntToString(m_rnamRenameExtenSettings.m_iRightCropNChar));

    prsuiRenameSettingsUI->m_pqcboExtenChangeCase->setCurrentIndex(m_rnamRenameExtenSettings.m_iChangeCase);
}


void IRenameSave::RestoreNumberingSettings(IUIRenameSettings* prsuiRenameSettingsUI)
{
    switch (m_rnumNumberingSettings.m_iNumberingType)
    {
    case IRenameNumber::NumberNoNumbering  : prsuiRenameSettingsUI->m_pqrbNumberingNoNumber->setChecked(true);
                                             break;
    case IRenameNumber::NumberAfterName    : prsuiRenameSettingsUI->m_pqrbNumberingAfterName->setChecked(true);
                                             break;
    case IRenameNumber::NumberBeforeName   : prsuiRenameSettingsUI->m_pqrbNumberingBeforeName->setChecked(true);
                                             break;
    case IRenameNumber::NumberAtPos        : prsuiRenameSettingsUI->m_pqrbNumberingAtPos->setChecked(true);
                                             break;
    }

    prsuiRenameSettingsUI->m_pqleNumberingAtPos->setText(ConvertIntToString(m_rnumNumberingSettings.m_iNumberingAtPos));

    prsuiRenameSettingsUI->m_pqleNumberingStartNum->setText(QString::number(m_rnumNumberingSettings.m_iNumberingStartNum));
    prsuiRenameSettingsUI->m_pqleNumberingIncrement->setText(QString::number(m_rnumNumberingSettings.m_iNumberingIncrement));

    if (m_rnumNumberingSettings.m_bNumberingZeroFillAuto)
        prsuiRenameSettingsUI->m_pqrbNumberingZeroFillAuto->setChecked(true);
    else
        prsuiRenameSettingsUI->m_pqrbNumberingZeroFillSpec->setChecked(true);
    prsuiRenameSettingsUI->m_pqleNumberingZeroFill->setText(QString::number(m_rnumNumberingSettings.m_iNumberingZeroFill));
}


int IRenameSave::ConvertIntForSaving(const QString & krqstrNumberToSave)
{
    if (krqstrNumberToSave.isEmpty())
        return m_kiEmptyBox;
    return krqstrNumberToSave.toInt();
}


QString IRenameSave::ConvertIntToString(const int kiNumber)
{
    if (kiNumber == m_kiEmptyBox)
        return "";
    return QString::number(kiNumber);
}


QString IRenameSave::OutputToString() const
{
    const int kiVersion = 1;
    QChar qchSeparator = SaveStringSeparator();

    return
    QString::number(kiVersion)                  % qchSeparator %
                    m_qstrName                  % qchSeparator %
    QString::number(m_iRenameElements)          % qchSeparator %
                    m_qstrRenameExtensions      %

    SaveStringSectionSeparator()                %
    m_rnamRenameNameSettings.OutputToString()   %
    SaveStringSectionSeparator()                %
    m_rnamRenameExtenSettings.OutputToString()  %
    SaveStringSectionSeparator()                %
    m_rnumNumberingSettings.OutputToString();
}


void IRenameSave::ReadFromString(const QString & krqstrInput)
{
    QStringList qstrlSections = krqstrInput.split(SaveStringSectionSeparator());
    QStringList qstrlValues = qstrlSections.at(0).split(SaveStringSeparator());

    int iIndex = 0;
    m_rnamRenameNameSettings.ReadFromString(qstrlSections.at(++iIndex));
    m_rnamRenameExtenSettings.ReadFromString(qstrlSections.at(++iIndex));
    m_rnumNumberingSettings.ReadFromString(qstrlSections.at(++iIndex));

    iIndex = 0;
    //int iVersion              = qstrlValues.at(iIndex).toInt();
    m_qstrName                  = qstrlValues.at(++iIndex);
    m_iRenameElements           = qstrlValues.at(++iIndex).toInt();
    m_qstrRenameExtensions      = qstrlValues.at(++iIndex);
}
