#ifndef IRenameInvalidCharSub_h
#define IRenameInvalidCharSub_h

#include <QSettings>
#include <QString>
class QTableWidget;
class IDlgPreferences;
class IComMetaMusic;
class IComMetaExif;


class IRenameInvalidCharSub
{
private:
    // Row indexes for each character in below strings, arrays and in Preferences table
    enum                CharIndexes {Slash, Backslash, Colon, Asterisk, QuestionMark, QuotationMark, LessThan, GreaterThan, VerticalBar, NumInvalidChars};
    const char*         m_rgszCharNames[NumInvalidChars] = {"Slash", "Backslash", "Colon", "Asterisk", "QuestionMark", "QuotationMark", "LessThan", "GreaterThan", "VerticalBar"};

    // Settings for loading and saving replacement characters
    QSettings &         m_rqsetSettings;

    // Invalid characters on Windows
    QString             m_qstrInvalidCharacters;

    // Default replacement characters on Windows and Linux/Mac
    QString             m_qstrDefaultReplacementsWindows;
    QString             m_qstrDefaultReplacementsLinuxMac;
    QString             m_qstrDefaultReplacements;

    // Flags indicating if characters should be replaced and replacement strings
    bool                m_rgbReplacementEnabled[NumInvalidChars];
    QString             m_rgqstrReplacment[NumInvalidChars];

    // Indicates changes were made to the sutstitute characters while the Preferences dialog was open
    bool                m_bChangesMade;

public:
    IRenameInvalidCharSub(QSettings & rqsetSettings);

    // Functions for populating table in Preferences dialog with invalid characters and character replacement settings
    void PopulateTableInvalidChars(QTableWidget* pqtwInvalidCharacterTable);
    void PopulateTableReplacements(QTableWidget* pqtwInvalidCharacterTable);
    void PopulateTableDefaultReplacements(QTableWidget* pqtwInvalidCharacterTable);
    void PopulateTableDefaultReplacementsWindows(QTableWidget* pqtwInvalidCharacterTable);

    // Checks if character replacement settings have changed and stores changed settings to this class and QSettings
    void SavePreferencesChanges(QTableWidget* pqtwInvalidCharacterTable);

    // Performs invalid character substitutions on passed string
    QString PerformSubstitution(QString qstrString);
    void PerformSubstitution(IComMetaMusic & rmmuMusicMeta);
    void PerformSubstitution(IComMetaExif & rmexExifMeta);

    // Accessor functions
    int GetNumInvalidChars()    {return NumInvalidChars;}
    bool ChangesMade()          {return m_bChangesMade;}
};

#endif // IRenameInvalidCharSub_h
