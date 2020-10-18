#include <QTableWidget>
#include "IRenameInvalidCharSub.h"
#include "IDlgPreferences.h"
#include "IComMetaMusic.h"
#include "IComMetaExif.h"
#include "IComQLineEdit.h"

/* Invalid characters on Windows:   /\:*?"<>|
 * https://unicode-search.net/unicode-namesearch.pl?term=slash
 * https://unicode-search.net/unicode-namesearch.pl?term=backslash
 * https://unicode-search.net/unicode-namesearch.pl?term=colon
 * https://unicode-search.net/unicode-namesearch.pl?term=asterisk
 * https://unicode-search.net/unicode-namesearch.pl?term=question
 * https://unicode-search.net/unicode-namesearch.pl?term=quotation
 * https://unicode-search.net/unicode-namesearch.pl?term=less%20than
 * https://unicode-search.net/unicode-namesearch.pl?term=greater%20than
 * https://unicode-search.net/unicode-namesearch.pl?term=bar
 */


IRenameInvalidCharSub::IRenameInvalidCharSub(QSettings & rqsetSettings) : m_rqsetSettings(rqsetSettings)
{
    // /\:*?"<>|
    m_qstrInvalidCharacters = "/\\:*?\"<>|";

    // On Windows all characters are replaced, while on Linux/Mac only slash is replaced by default
    m_qstrDefaultReplacementsWindows = "⁄﹨˸＊？“＜＞¦";
    m_qstrDefaultReplacementsLinuxMac = "⁄\\:*?\"<>|";

    #ifdef Q_OS_WIN
    m_qstrDefaultReplacements = m_qstrDefaultReplacementsWindows;
    #else
    m_qstrDefaultReplacements = m_qstrDefaultReplacementsLinuxMac;
    #endif

    m_rqsetSettings.beginGroup("CharacterSubtitutions");
    for (int iIndex = 0 ; iIndex < NumInvalidChars ; ++iIndex)
    {
        m_rgqstrReplacment[iIndex] = m_rqsetSettings.value(m_rgszCharNames[iIndex], m_qstrDefaultReplacements.at(iIndex)).toString();
        m_rgbReplacementEnabled[iIndex] = !(m_rgqstrReplacment[iIndex].size() == 1 && m_rgqstrReplacment[iIndex].at(0) == m_qstrInvalidCharacters.at(iIndex));
    }
    m_rqsetSettings.endGroup();
}


void IRenameInvalidCharSub::PopulateTableInvalidChars(QTableWidget* pqtwInvalidCharacterTable)
{
    for (int iIndex = 0 ; iIndex < NumInvalidChars ; ++iIndex)
        pqtwInvalidCharacterTable->item(iIndex, 0)->setText(m_qstrInvalidCharacters.at(iIndex));
}


void IRenameInvalidCharSub::PopulateTableReplacements(QTableWidget* pqtwInvalidCharacterTable)
{
    for (int iIndex = 0 ; iIndex < NumInvalidChars ; ++iIndex)
        dynamic_cast<IComQLineEdit*>(pqtwInvalidCharacterTable->cellWidget(iIndex, 1))->setText(m_rgqstrReplacment[iIndex]);
}


void IRenameInvalidCharSub::PopulateTableDefaultReplacements(QTableWidget* pqtwInvalidCharacterTable)
{
    for (int iIndex = 0 ; iIndex < NumInvalidChars ; ++iIndex)
        dynamic_cast<IComQLineEdit*>(pqtwInvalidCharacterTable->cellWidget(iIndex, 1))->setText(m_qstrDefaultReplacements.at(iIndex));
}


void IRenameInvalidCharSub::PopulateTableDefaultReplacementsWindows(QTableWidget* pqtwInvalidCharacterTable)
{
    for (int iIndex = 0 ; iIndex < NumInvalidChars ; ++iIndex)
        dynamic_cast<IComQLineEdit*>(pqtwInvalidCharacterTable->cellWidget(iIndex, 1))->setText(m_qstrDefaultReplacementsWindows.at(iIndex));
}


void IRenameInvalidCharSub::SavePreferencesChanges(QTableWidget* pqtwInvalidCharacterTable)
{
    m_bChangesMade = false;
    QString qstrTableVal;
    m_rqsetSettings.beginGroup("CharacterSubtitutions");
    for (int iIndex = 0 ; iIndex < NumInvalidChars ; ++iIndex)
    {
        qstrTableVal = dynamic_cast<IComQLineEdit*>(pqtwInvalidCharacterTable->cellWidget(iIndex, 1))->text();
        if (qstrTableVal != m_rgqstrReplacment[iIndex])
        {
            m_bChangesMade = true;
            m_rgqstrReplacment[iIndex] = qstrTableVal;
            m_rqsetSettings.setValue(m_rgszCharNames[iIndex], qstrTableVal);
        }
        m_rgbReplacementEnabled[iIndex] = !(m_rgqstrReplacment[iIndex].size() == 1 && m_rgqstrReplacment[iIndex].at(0) == m_qstrInvalidCharacters.at(iIndex));
    }
    m_rqsetSettings.endGroup();
}


QString IRenameInvalidCharSub::PerformSubstitution(QString qstrString)
{
    for (int iIndex = 0 ; iIndex < NumInvalidChars ; ++iIndex)
    {
        if (m_rgbReplacementEnabled[iIndex])
            qstrString.replace(m_qstrInvalidCharacters.at(iIndex), m_rgqstrReplacment[iIndex]);
    }
    return qstrString;
}


void IRenameInvalidCharSub::PerformSubstitution(IComMetaMusic & rmmuMusicMeta)
{
    rmmuMusicMeta.SetTitle  (PerformSubstitution(rmmuMusicMeta.GetTitle()));
    rmmuMusicMeta.SetArtist (PerformSubstitution(rmmuMusicMeta.GetArtist()));
    rmmuMusicMeta.SetAlbum  (PerformSubstitution(rmmuMusicMeta.GetAlbum()));
    rmmuMusicMeta.SetGenre  (PerformSubstitution(rmmuMusicMeta.GetGenre()));
    rmmuMusicMeta.SetComment(PerformSubstitution(rmmuMusicMeta.GetComment()));
}


void IRenameInvalidCharSub::PerformSubstitution(IComMetaExif & /*rmexExifMeta*/)
{
    /* The only Exif tags that may contain invalid characters are make, model and program.
     * Of them only model is likely to contain invalid characters.  I don't think it's worth bothering.
    PerformSubstitution(rmexExifMeta.GetCameraMake());
    PerformSubstitution(rmexExifMeta.GetCameraModel());
    PerformSubstitution(rmexExifMeta.GetProgram());*/
}
