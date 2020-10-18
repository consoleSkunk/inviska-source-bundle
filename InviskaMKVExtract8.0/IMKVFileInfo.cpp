#include <QtWidgets>
#include "IMKVFileInfo.h"
#include "IUIExtract.h"


IMKVFileInfo::IMKVFileInfo()
{
    Reset();
}


void IMKVFileInfo::Reset()
{
    m_iVideoTrackCount    = 0;
    m_iAudioTrackCount    = 0;
    m_iSubtitleTrackCount = 0;
    m_iAttatchmentCount   = 0;
    m_bHasChapters        = false;
    m_bHasCuesheet        = false;
    m_bHasTags            = false;
}


void IMKVFileInfo::UpdateTrackCounts(const int kiTrackType)
{
    switch (kiTrackType)
    {
    case IUIExtract::Video      :   ++m_iVideoTrackCount;
                                    break;
    case IUIExtract::Audio      :   ++m_iAudioTrackCount;
                                    break;
    case IUIExtract::Subtitles  :   ++m_iSubtitleTrackCount;
                                    break;
    case IUIExtract::Attachment :   ++m_iAttatchmentCount;
                                    break;
    case IUIExtract::Chapters   :   m_bHasChapters = true;
                                    break;
    case IUIExtract::Cuesheet   :   m_bHasCuesheet = true;
                                    break;
    case IUIExtract::Tags       :   m_bHasTags     = true;
                                    break;
    }
}


int IMKVFileInfo::GetTrackCountForType(const int kiTrackType)
{
    switch (kiTrackType)
    {
    case IUIExtract::Video      :   return m_iVideoTrackCount;
    case IUIExtract::Audio      :   return m_iAudioTrackCount;
    case IUIExtract::Subtitles  :   return m_iSubtitleTrackCount;
    case IUIExtract::Attachment :   return m_iAttatchmentCount;
    case IUIExtract::Chapters   :   return m_bHasChapters ? 1 : 0;
    case IUIExtract::Cuesheet   :   return m_bHasCuesheet ? 1 : 0;
    case IUIExtract::Tags       :   return m_bHasTags     ? 1 : 0;
    }

    return 0;
}


void IMKVFileInfo::UpdateFileMaxValues(const IMKVFileInfo & krfinfFileInfo, IUIExtract* puiextExtractUI)
{
    if (krfinfFileInfo.m_iVideoTrackCount > m_iVideoTrackCount)
    {
        while (m_iVideoTrackCount < krfinfFileInfo.m_iVideoTrackCount)
        {
            ++m_iVideoTrackCount;
            puiextExtractUI->AddToBatchExtractList(tr("Video Track %1").arg(m_iVideoTrackCount), IUIExtract::Video+m_iVideoTrackCount);
        }
    }

    if (krfinfFileInfo.m_iAudioTrackCount > m_iAudioTrackCount)
    {
        while (m_iAudioTrackCount < krfinfFileInfo.m_iAudioTrackCount)
        {
            ++m_iAudioTrackCount;
            puiextExtractUI->AddToBatchExtractList(tr("Audio Track %1").arg(m_iAudioTrackCount), IUIExtract::Audio+m_iAudioTrackCount);
        }
    }

    if (krfinfFileInfo.m_iSubtitleTrackCount > m_iSubtitleTrackCount)
    {
        while (m_iSubtitleTrackCount < krfinfFileInfo.m_iSubtitleTrackCount)
        {
            ++m_iSubtitleTrackCount;
            puiextExtractUI->AddToBatchExtractList(tr("Subtitle Track %1").arg(m_iSubtitleTrackCount), IUIExtract::Subtitles+m_iSubtitleTrackCount);
        }
    }

    if (m_bHasChapters == false && krfinfFileInfo.m_bHasChapters == true)
    {
        m_bHasChapters = true;
        puiextExtractUI->AddToBatchExtractList(tr("Chapters"), IUIExtract::Chapters);
    }

    if (m_bHasCuesheet == false && krfinfFileInfo.m_bHasCuesheet == true)
    {
        m_bHasCuesheet = true;
        puiextExtractUI->AddToBatchExtractList(tr("Cuesheet"), IUIExtract::Cuesheet);
    }

    if (m_bHasTags == false && krfinfFileInfo.m_bHasTags == true)
    {
        m_bHasTags = true;
        puiextExtractUI->AddToBatchExtractList(tr("Tags"), IUIExtract::Tags);
    }

    if (m_iAttatchmentCount == 0 && krfinfFileInfo.m_iAttatchmentCount != 0)
    {
        m_iAttatchmentCount = 1;
        puiextExtractUI->AddToBatchExtractList(tr("Attachments"), IUIExtract::AttachmentsGroup);
    }
}
