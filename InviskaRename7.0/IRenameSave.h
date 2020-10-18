#ifndef IRenameSave_h
#define IRenameSave_h

#include <QObject>
#include "IRenameName.h"
#include "IRenameNumber.h"
class IUIRenameSettings;


class IRenameSave
{
public:
    QString                     m_qstrName;
    int                         m_iRenameElements;
    QString                     m_qstrRenameExtensions;
    IRenameName                 m_rnamRenameNameSettings;
    IRenameName                 m_rnamRenameExtenSettings;
    IRenameNumber               m_rnumNumberingSettings;

    static const int            m_kiEmptyBox = -1;

public:
    void SaveCurrentSettings(const QString & krqstrName, IUIRenameSettings* prsuiRenameSettingsUI);

private:
    void SaveNameSettings(IUIRenameSettings* prsuiRenameSettingsUI);
    void SaveExtensionSettings(IUIRenameSettings* prsuiRenameSettingsUI);
    void SaveNumberingSettings(IUIRenameSettings* prsuiRenameSettingsUI);

public:
    void RestoreSettings(IUIRenameSettings* prsuiRenameSettingsUI);

private:
    void RestoreNameSettings(IUIRenameSettings* prsuiRenameSettingsUI);
    void RestoreExtensionSettings(IUIRenameSettings* prsuiRenameSettingsUI);
    void RestoreNumberingSettings(IUIRenameSettings* prsuiRenameSettingsUI);

    int ConvertIntForSaving(const QString & krqstrNumberToSave);
    QString ConvertIntToString(const int kiNumber);

public:
    // Returns seperator for items in save string, and separator for sections e.g. name, extension, numbering
    static QChar   SaveStringSeparator()        {return '/';}
    static QString SaveStringSectionSeparator() {return "/|/";}

    // Functions for saving the data to, and writing it from, a QString for strong in the settings file
    QString OutputToString() const;
    void    ReadFromString(const QString & krqstrInput);
};


Q_DECLARE_METATYPE(IRenameSave*)

#endif // IRenameSave_h
