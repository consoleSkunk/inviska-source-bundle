#include "IMetaMusic.h"
#include "IComMetaMusic.h"


IMetaMusic::IMetaMusic()
{

}


IMetaMusic::IMetaMusic(IComMetaMusic* pmmuMusicMeta)
{
    m_qstrTitle         = pmmuMusicMeta->GetTitle();
    m_qstrArtist        = pmmuMusicMeta->GetArtist();
    m_qstrAlbum         = pmmuMusicMeta->GetAlbum();
    m_qstrTrack         = pmmuMusicMeta->GetTrackTwoDigit();
    m_qstrYear          = pmmuMusicMeta->GetYear();
    m_qstrGenre         = pmmuMusicMeta->GetGenre();
    m_qstrComment       = pmmuMusicMeta->GetComment();
    m_qstrRunTime       = pmmuMusicMeta->GetLengthMMSSSemiColon();
    m_qstrChannels      = pmmuMusicMeta->GetChannels();
    m_qstrSampleRate    = pmmuMusicMeta->GetSampleRate();
    m_qstrBitRate       = pmmuMusicMeta->GetBitRate();
}
