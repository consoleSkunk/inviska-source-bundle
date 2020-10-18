#include <QtWidgets>
#include <QAction>
#include "IUIMenuBar.h"
#include "IUIMainWindow.h"
#include "IUIFileList.h"
#include "IUIRenameSettings.h"
#include "IUISideBar.h"
#include "IRenameSave.h"


IUIMenuBar::IUIMenuBar(IUIMainWindow* pmwMainWindow) : IComUIMenuBarBase(pmwMainWindow)
{
    CreateActions();
    InitialiseMenuBar();
}


void IUIMenuBar::CreateActions()
{
    CreateFileActions();
    CreateNavigateActions();
    CreateTagsActionsMusic();
    CreateTagsActionsExif();
    CreateTagsActionsFileAttributes();
    CreateRenamesActions();
    IComUIMenuBarBase::CreateActions();
}


void IUIMenuBar::CreateFileActions()
{
    IUIFileList* & puifmFilelist = m_pmwMainWindow->GetFileListUI();

    m_pqactOpenDirectory = new QAction(tr("&Open Directory..."), m_pmwMainWindow);
    m_pqactOpenDirectory->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_DirOpenIcon));
    m_pqactOpenDirectory->setShortcut(QKeySequence::Open);
    QObject::connect(m_pqactOpenDirectory, SIGNAL(triggered()), puifmFilelist, SLOT(OpenDirectoryDialog()));

    m_pqactViewFileMetaTags = new QAction(tr("&View File Properties && Tags..."), m_pmwMainWindow);
    m_pqactViewFileMetaTags->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_FileDialogContentsView));
    QObject::connect(m_pqactViewFileMetaTags, SIGNAL(triggered()), puifmFilelist, SLOT(OpenFilePropertiesDialog()));

    m_pqactCaseSensitiveCompare = new QAction(tr("&Case Sensitive Comparisons"), m_pmwMainWindow);
    m_pqactCaseSensitiveCompare->setCheckable(true);
    m_pqactCaseSensitiveCompare->setChecked(m_pmwMainWindow->GetSettings().value("FileList/CaseSensitiveCompare", false).toBool());
    m_pqactCaseSensitiveCompare->setIcon(QIcon(":/Resources/CaseSensitive.png"));
    m_pqactCaseSensitiveCompare->setToolTip(tr("Enable/disable case-sensitive comparisons.\nApplies to \"Replace The Text\" and\"Files With Extensions\"."));
    QObject::connect(m_pqactCaseSensitiveCompare, SIGNAL(triggered()), puifmFilelist, SLOT(SetCaseSensitiveCompare()));
}


void IUIMenuBar::CreateNavigateActions()
{
    IUIFileList* & puifmFilelist = m_pmwMainWindow->GetFileListUI();

    m_pqactNavigateBack = new QAction(tr("&Back"), m_pmwMainWindow);
    m_pqactNavigateBack->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_ArrowLeft));
    m_pqactNavigateBack->setShortcut(QKeySequence::Back);
    QObject::connect(m_pqactNavigateBack, SIGNAL(triggered()), puifmFilelist, SLOT(NavigateBack()));

    m_pqactNavigateForward = new QAction(tr("&Forward"), m_pmwMainWindow);
    m_pqactNavigateForward->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_ArrowRight));
    m_pqactNavigateForward->setShortcut(QKeySequence::Forward);
    QObject::connect(m_pqactNavigateForward, SIGNAL(triggered()), puifmFilelist, SLOT(NavigateForward()));

    m_pqactNavigateUp = new QAction(tr("&Up"), m_pmwMainWindow);
    m_pqactNavigateUp->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_ArrowUp));
    m_pqactNavigateUp->setShortcut(tr("Alt+Up"));
    QObject::connect(m_pqactNavigateUp, SIGNAL(triggered()), puifmFilelist, SLOT(NavigateUp()));

    m_pqactNavigateRefresh = new QAction(tr("&Refresh"), m_pmwMainWindow);
    m_pqactNavigateRefresh->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_BrowserReload));
    m_pqactNavigateRefresh->setShortcut(QKeySequence::Refresh);
    QObject::connect(m_pqactNavigateRefresh, SIGNAL(triggered()), puifmFilelist, SLOT(NavigateRefresh()));

    m_pqactNavigateStartDir = new QAction(tr("Go To &Start Directory"), m_pmwMainWindow);
    m_pqactNavigateStartDir->setIcon(QIcon(":/Resources/Home.png"));
    m_pqactNavigateStartDir->setShortcut(tr("Alt+Home"));
    QObject::connect(m_pqactNavigateStartDir, SIGNAL(triggered()), puifmFilelist, SLOT(NavigateToStartDirectory()));

    m_pqactShowHiddenFiles = new QAction(tr("Show &Hidden Files"), m_pmwMainWindow);
    m_pqactShowHiddenFiles->setCheckable(true);
    m_pqactShowHiddenFiles->setChecked(m_pmwMainWindow->GetSettings().value("FileList/ShowHiddenFiles", false).toBool());
    m_pqactShowHiddenFiles->setIcon(QIcon(":/Resources/ShowHidden.png"));
    m_pqactShowHiddenFiles->setShortcut(tr("Ctrl+H"));
    m_pqactShowHiddenFiles->setToolTip(tr("Show/hide hidden files"));
    QObject::connect(m_pqactShowHiddenFiles, SIGNAL(triggered()), puifmFilelist, SLOT(SetHiddenFileState()));

    m_pqactMoveSelectedLinesUp = new QAction(tr("Move Selected Lines Up"), this);
    m_pqactMoveSelectedLinesUp->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_ArrowUp));
    m_pqactMoveSelectedLinesUp->setShortcut(tr("Ctrl+Shift+Up"));
    QObject::connect(m_pqactMoveSelectedLinesUp, SIGNAL(triggered()), puifmFilelist, SLOT(MoveSelectionUp()));

    m_pqactMoveSelectedLinesDown = new QAction(tr("Move Selected Lines Down"), this);
    m_pqactMoveSelectedLinesDown->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_ArrowDown));
    m_pqactMoveSelectedLinesDown->setShortcut(tr("Ctrl+Shift+Down"));
    QObject::connect(m_pqactMoveSelectedLinesDown, SIGNAL(triggered()), puifmFilelist, SLOT(MoveSelectionDown()));
}


void IUIMenuBar::CreateTagsActionsMusic()
{
    m_pqactTitle                = new QAction(tr("Title%1").arg(PrintTagOS("\t[$Mu-Title]")), m_pmwMainWindow);
    m_pqactArtist               = new QAction(tr("Artist%1").arg(PrintTagOS("\t[$Mu-Artist]")), m_pmwMainWindow);
    m_pqactAlbum                = new QAction(tr("Album%1").arg(PrintTagOS("\t[$Mu-Album]")), m_pmwMainWindow);
    m_pqactTrack                = new QAction(tr("Track%1").arg(PrintTagOS("\t[$Mu-Track]")), m_pmwMainWindow);
    m_pqactYear                 = new QAction(tr("Year%1").arg(PrintTagOS("\t[$Mu-Year]")), m_pmwMainWindow);
    m_pqactGenre                = new QAction(tr("Genre%1").arg(PrintTagOS("\t[$Mu-Genre]")), m_pmwMainWindow);
    m_pqactComment              = new QAction(tr("Comment%1").arg(PrintTagOS("\t[$Mu-Comment]")), m_pmwMainWindow);
    m_pqactRunTime              = new QAction(tr("Run Time%1").arg(PrintTagOS("\t[$Mu-RunTime]")), m_pmwMainWindow);
    m_pqactChannels             = new QAction(tr("Channels%1").arg(PrintTagOS("\t[$Mu-Channels]")), m_pmwMainWindow);
    m_pqactSampleRate           = new QAction(tr("Sample Rate Hz%1").arg(PrintTagOS("\t[$Mu-SampleRate]")), m_pmwMainWindow);
    m_pqactBitRate              = new QAction(tr("Bit Rate kbps%1").arg(PrintTagOS("\t[$Mu-BitRate]")), m_pmwMainWindow);

    IUIRenameSettings* & puirsRenameSettingsUI = m_pmwMainWindow->GetRenameSettingsUI();
    puirsRenameSettingsUI->AddActionLookupMusic(m_pqactTitle,           "[$Mu-Title]");
    puirsRenameSettingsUI->AddActionLookupMusic(m_pqactArtist,          "[$Mu-Artist]");
    puirsRenameSettingsUI->AddActionLookupMusic(m_pqactAlbum,           "[$Mu-Album]");
    puirsRenameSettingsUI->AddActionLookupMusic(m_pqactTrack,           "[$Mu-Track]");
    puirsRenameSettingsUI->AddActionLookupMusic(m_pqactYear,            "[$Mu-Year]");
    puirsRenameSettingsUI->AddActionLookupMusic(m_pqactGenre,           "[$Mu-Genre]");
    puirsRenameSettingsUI->AddActionLookupMusic(m_pqactComment,         "[$Mu-Comment]");
    puirsRenameSettingsUI->AddActionLookupMusic(m_pqactRunTime,         "[$Mu-RunTime]");
    puirsRenameSettingsUI->AddActionLookupMusic(m_pqactChannels,        "[$Mu-Channels]");
    puirsRenameSettingsUI->AddActionLookupMusic(m_pqactSampleRate,      "[$Mu-SampleRate]");
    puirsRenameSettingsUI->AddActionLookupMusic(m_pqactBitRate,         "[$Mu-BitRate]");
}


void IUIMenuBar::CreateTagsActionsExif()
{
    m_pqactTimeSubSec           = new QAction(tr("Time Sub-Second%1").arg(PrintTagOS("\t[$Ex-TimeSubSec]")), m_pmwMainWindow);
    m_pqactCameraMake           = new QAction(tr("Camera Make%1").arg(PrintTagOS("\t[$Ex-CameraMake]")), m_pmwMainWindow);
    m_pqactCameraModel          = new QAction(tr("Camera Model%1").arg(PrintTagOS("\t[$Ex-CameraModel]")), m_pmwMainWindow);
    m_pqactFNumber              = new QAction(tr("F-Number%1").arg(PrintTagOS("\t[$Ex-FNumber]")), m_pmwMainWindow);
    m_pqactISOSpeed             = new QAction(tr("ISO Speed%1").arg(PrintTagOS("\t[$Ex-ISOSpeed]")), m_pmwMainWindow);
    m_pqactExposureTime         = new QAction(tr("Exposure Time%1").arg(PrintTagOS("\t[$Ex-ExposureTime]")), m_pmwMainWindow);
    m_pqactExposureTimeDec      = new QAction(tr("Exposure Time Decimal%1").arg(PrintTagOS("\t[$Ex-ExposureTimeDec]")), m_pmwMainWindow);
    m_pqactFocalLength          = new QAction(tr("Focal Length%1").arg(PrintTagOS("\t[$Ex-FocalLength]")), m_pmwMainWindow);
    m_pqactProgram              = new QAction(tr("Program%1").arg(PrintTagOS("\t[$Ex-Program]")), m_pmwMainWindow);
    m_pqactPixelDimX            = new QAction(tr("Pixel Dimension X%1").arg(PrintTagOS("\t[$Ex-PixelDimX]")), m_pmwMainWindow);
    m_pqactPixelDimY            = new QAction(tr("Pixel Dimension Y%1").arg(PrintTagOS("\t[$Ex-PixelDimY]")), m_pmwMainWindow);
    m_pqactDateTime             = new QAction(tr("Date Time%1").arg(PrintTagOS("\t[$Ex-DateTime]")), m_pmwMainWindow);
    m_pqactDate                 = new QAction(tr("Date%1").arg(PrintTagOS("\t[$Ex-Date]")), m_pmwMainWindow);
    m_pqactTime                 = new QAction(tr("Time%1").arg(PrintTagOS("\t[$Ex-Time]")), m_pmwMainWindow);
    m_pqactDateYYYY             = new QAction(tr("Date YYYY%1").arg(PrintTagOS("\t[$Ex-DateYYYY]")), m_pmwMainWindow);
    m_pqactDateYY               = new QAction(tr("Date YY%1").arg(PrintTagOS("\t[$Ex-DateYY]")), m_pmwMainWindow);
    m_pqactDateMM               = new QAction(tr("Date MM%1").arg(PrintTagOS("\t[$Ex-DateMM]")), m_pmwMainWindow);
    m_pqactDateDD               = new QAction(tr("Date DD%1").arg(PrintTagOS("\t[$Ex-DateDD]")), m_pmwMainWindow);
    m_pqactTimeHH               = new QAction(tr("Time HH%1").arg(PrintTagOS("\t[$Ex-TimeHH]")), m_pmwMainWindow);
    m_pqactTimeMM               = new QAction(tr("Time MM%1").arg(PrintTagOS("\t[$Ex-TimeMM]")), m_pmwMainWindow);
    m_pqactTimeSS               = new QAction(tr("Time SS%1").arg(PrintTagOS("\t[$Ex-TimeSS]")), m_pmwMainWindow);

    IUIRenameSettings* & puirsRenameSettingsUI = m_pmwMainWindow->GetRenameSettingsUI();
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactCameraMake,       "[$Ex-CameraMake]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactCameraModel,      "[$Ex-CameraModel]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactFNumber,          "[$Ex-FNumber]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactISOSpeed,         "[$Ex-ISOSpeed]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactExposureTime,     "[$Ex-ExposureTime]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactExposureTimeDec,  "[$Ex-ExposureTimeDec]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactFocalLength,      "[$Ex-FocalLength]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactProgram,          "[$Ex-Program]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactPixelDimX,        "[$Ex-PixelDimX]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactPixelDimY,        "[$Ex-PixelDimY]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactDateTime,         "[$Ex-DateTime]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactDate,             "[$Ex-Date]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactTime,             "[$Ex-Time]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactDateYYYY,         "[$Ex-DateYYYY]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactDateYY,           "[$Ex-DateYY]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactDateMM,           "[$Ex-DateMM]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactDateDD,           "[$Ex-DateDD]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactTimeHH,           "[$Ex-TimeHH]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactTimeMM,           "[$Ex-TimeMM]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactTimeSS,           "[$Ex-TimeSS]");
    puirsRenameSettingsUI->AddActionLookupExif(m_pqactTimeSubSec,       "[$Ex-TimeSubSec]");
}


void IUIMenuBar::CreateTagsActionsFileAttributes()
{
    m_pqactFiCreatedDateTime    = new QAction(tr("Created Date Time%1").arg(PrintTagOS("\t[$At-CreatedDateTime]")), m_pmwMainWindow);
    m_pqactFiCreatedDate        = new QAction(tr("Created Date%1").arg(PrintTagOS("\t[$At-CreatedDate]")), m_pmwMainWindow);
    m_pqactFiCreatedTime        = new QAction(tr("Created Time%1").arg(PrintTagOS("\t[$At-CreatedTime]")), m_pmwMainWindow);
    m_pqactFiCreatedDateYYYY    = new QAction(tr("Created Date YYYY%1").arg(PrintTagOS("\t[$At-CreatedDateYYYY]")), m_pmwMainWindow);
    m_pqactFiCreatedDateYY      = new QAction(tr("Created Date YY%1").arg(PrintTagOS("\t[$At-CreatedDateYY]")), m_pmwMainWindow);
    m_pqactFiCreatedDateMM      = new QAction(tr("Created Date MM%1").arg(PrintTagOS("\t[$At-CreatedDateMM]")), m_pmwMainWindow);
    m_pqactFiCreatedDateDD      = new QAction(tr("Created Date DD%1").arg(PrintTagOS("\t[$At-CreatedDateDD]")), m_pmwMainWindow);
    m_pqactFiCreatedTimeHH      = new QAction(tr("Created Time HH%1").arg(PrintTagOS("\t[$At-CreatedTimeHH]")), m_pmwMainWindow);
    m_pqactFiCreatedTimeMM      = new QAction(tr("Created Time MM%1").arg(PrintTagOS("\t[$At-CreatedTimeMM]")), m_pmwMainWindow);
    m_pqactFiCreatedTimeSS      = new QAction(tr("Created Time SS%1").arg(PrintTagOS("\t[$At-CreatedTimeSS]")), m_pmwMainWindow);
    m_pqactFiModifiedDateTime   = new QAction(tr("Modified Date Time%1").arg(PrintTagOS("\t[$At-ModifiedDateTime]")), m_pmwMainWindow);
    m_pqactFiModifiedDate       = new QAction(tr("Modified Date%1").arg(PrintTagOS("\t[$At-ModifiedDate]")), m_pmwMainWindow);
    m_pqactFiModifiedTime       = new QAction(tr("Modified Time%1").arg(PrintTagOS("\t[$At-ModifiedTime]")), m_pmwMainWindow);
    m_pqactFiModifiedDateYYYY   = new QAction(tr("Modified Date YYYY%1").arg(PrintTagOS("\t[$At-ModifiedDateYYYY]")), m_pmwMainWindow);
    m_pqactFiModifiedDateYY     = new QAction(tr("Modified Date YY%1").arg(PrintTagOS("\t[$At-ModifiedDateYY]")), m_pmwMainWindow);
    m_pqactFiModifiedDateMM     = new QAction(tr("Modified Date MM%1").arg(PrintTagOS("\t[$At-ModifiedDateMM]")), m_pmwMainWindow);
    m_pqactFiModifiedDateDD     = new QAction(tr("Modified Date DD%1").arg(PrintTagOS("\t[$At-ModifiedDateDD]")), m_pmwMainWindow);
    m_pqactFiModifiedTimeHH     = new QAction(tr("Modified Time HH%1").arg(PrintTagOS("\t[$At-ModifiedTimeHH]")), m_pmwMainWindow);
    m_pqactFiModifiedTimeMM     = new QAction(tr("Modified Time MM%1").arg(PrintTagOS("\t[$At-ModifiedTimeMM]")), m_pmwMainWindow);
    m_pqactFiModifiedTimeSS     = new QAction(tr("Modified Time SS%1").arg(PrintTagOS("\t[$At-ModifiedTimeSS]")), m_pmwMainWindow);

    IUIRenameSettings* & puirsRenameSettingsUI = m_pmwMainWindow->GetRenameSettingsUI();
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiCreatedDateTime,"[$At-CreatedDateTime]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiCreatedDate,    "[$At-CreatedDate]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiCreatedTime,    "[$At-CreatedTime]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiCreatedDateYYYY,"[$At-CreatedDateYYYY]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiCreatedDateYY,  "[$At-CreatedDateYY]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiCreatedDateMM,  "[$At-CreatedDateMM]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiCreatedDateDD,  "[$At-CreatedDateDD]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiCreatedTimeHH,  "[$At-CreatedTimeHH]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiCreatedTimeMM,  "[$At-CreatedTimeMM]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiCreatedTimeSS,  "[$At-CreatedTimeSS]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiModifiedDateTime,"[$At-ModifiedDateTime]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiModifiedDate,   "[$At-ModifiedDate]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiModifiedTime,   "[$At-ModifiedTime]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiModifiedDateYYYY,"[$At-ModifiedDateYYYY]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiModifiedDateYY, "[$At-ModifiedDateYY]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiModifiedDateMM, "[$At-ModifiedDateMM]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiModifiedDateDD, "[$At-ModifiedDateDD]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiModifiedTimeHH, "[$At-ModifiedTimeHH]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiModifiedTimeMM, "[$At-ModifiedTimeMM]");
    puirsRenameSettingsUI->AddActionLookupFiAt(m_pqactFiModifiedTimeSS, "[$At-ModifiedTimeSS]");
}


void IUIMenuBar::CreateRenamesActions()
{
    IUIRenameSettings* & puirsRenameSettingsUI = m_pmwMainWindow->GetRenameSettingsUI();

    m_pqactSaveCurrentSettings = new QAction(tr("&Save Rename Settings..."), m_pmwMainWindow);
    m_pqactSaveCurrentSettings->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_DialogSaveButton));
    m_pqactSaveCurrentSettings->setShortcut(QKeySequence::Save);
    QObject::connect(m_pqactSaveCurrentSettings, SIGNAL(triggered()), puirsRenameSettingsUI, SLOT(ShowSaveRenameSettingsDialog()));

    m_pqactOrganiseSavedRenames = new QAction(tr("&Organise Saved Renames..."), m_pmwMainWindow);
    m_pqactOrganiseSavedRenames->setIcon(m_pmwMainWindow->style()->standardIcon(QStyle::SP_DirHomeIcon));
    m_pqactOrganiseSavedRenames->setShortcut(tr("Ctrl+R"));
    QObject::connect(m_pqactOrganiseSavedRenames, SIGNAL(triggered()), puirsRenameSettingsUI, SLOT(ShowOrganiseSavedRenamesDialog()));

    m_pqactClearRenameSettings = new QAction(tr("&Clear Current Settings"), m_pmwMainWindow);
    m_pqactClearRenameSettings->setShortcut(tr("Ctrl+D"));
    QObject::connect(m_pqactClearRenameSettings, SIGNAL(triggered()), puirsRenameSettingsUI, SLOT(ClearAll()));
}



void IUIMenuBar::InitialiseMenuBar()
{
    m_pqmenuFile = m_pmwMainWindow->menuBar()->addMenu(tr("&File"));
    m_pqmenuFile->addAction(m_pqactOpenDirectory);
    m_pqmenuFile->addAction(m_pqactViewFileMetaTags);
    m_pqmenuFile->addAction(m_pqactPreferences);
    m_pqmenuFile->addAction(m_pqactCaseSensitiveCompare);
    m_pqmenuFile->addSeparator();
    m_pqmenuFile->addAction(m_pqactExit);

    m_pqmenuNavigate = m_pmwMainWindow->menuBar()->addMenu(tr("&Navigation"));
    m_pqmenuNavigate->addAction(m_pqactNavigateStartDir);
    m_pqmenuNavigate->addAction(m_pqactNavigateBack);
    m_pqmenuNavigate->addAction(m_pqactNavigateForward);
    m_pqmenuNavigate->addAction(m_pqactNavigateUp);
    m_pqmenuNavigate->addAction(m_pqactNavigateRefresh);
    m_pqmenuNavigate->addSeparator();
    m_pqmenuNavigate->addAction(m_pqactShowHiddenFiles);
    m_pqmenuNavigate->addSeparator();
    m_pqmenuNavigate->addAction(m_pqactMoveSelectedLinesUp);
    m_pqmenuNavigate->addAction(m_pqactMoveSelectedLinesDown);

    m_pqmenuTags = m_pmwMainWindow->menuBar()->addMenu(tr("&Tags"));
    m_pqmenuMusic = m_pqmenuTags->addMenu("&Music");
    m_pqmenuMusic->addAction(m_pqactTitle);
    m_pqmenuMusic->addAction(m_pqactArtist);
    m_pqmenuMusic->addAction(m_pqactAlbum);
    m_pqmenuMusic->addAction(m_pqactTrack);
    m_pqmenuMusic->addAction(m_pqactYear);
    m_pqmenuMusic->addAction(m_pqactGenre);
    m_pqmenuMusic->addAction(m_pqactComment);
    m_pqmenuMusic->addAction(m_pqactRunTime);
    m_pqmenuMusic->addAction(m_pqactChannels);
    m_pqmenuMusic->addAction(m_pqactSampleRate);
    m_pqmenuMusic->addAction(m_pqactBitRate);

    m_pqmenuExif = m_pqmenuTags->addMenu("&Exif");
    m_pqmenuExif->addAction(m_pqactDateTime);
    m_pqmenuExif->addAction(m_pqactDate);
    m_pqmenuExif->addAction(m_pqactTime);
    m_pqmenuExif->addAction(m_pqactDateYYYY);
    m_pqmenuExif->addAction(m_pqactDateYY);
    m_pqmenuExif->addAction(m_pqactDateMM);
    m_pqmenuExif->addAction(m_pqactDateDD);
    m_pqmenuExif->addAction(m_pqactTimeHH);
    m_pqmenuExif->addAction(m_pqactTimeMM);
    m_pqmenuExif->addAction(m_pqactTimeSS);
    m_pqmenuExif->addAction(m_pqactTimeSubSec);
    m_pqmenuExif->addAction(m_pqactCameraMake);
    m_pqmenuExif->addAction(m_pqactCameraModel);
    m_pqmenuExif->addAction(m_pqactFNumber);
    m_pqmenuExif->addAction(m_pqactISOSpeed);
    m_pqmenuExif->addAction(m_pqactExposureTime);
    m_pqmenuExif->addAction(m_pqactExposureTimeDec);
    m_pqmenuExif->addAction(m_pqactFocalLength);
    m_pqmenuExif->addAction(m_pqactProgram);
    m_pqmenuExif->addAction(m_pqactPixelDimX);
    m_pqmenuExif->addAction(m_pqactPixelDimY);

    m_pqmenuAttributes = m_pqmenuTags->addMenu("&Attributes");
    m_pqmenuAttributes->addAction(m_pqactFiCreatedDateTime);
    m_pqmenuAttributes->addAction(m_pqactFiCreatedDate);
    m_pqmenuAttributes->addAction(m_pqactFiCreatedTime);
    m_pqmenuAttributes->addAction(m_pqactFiCreatedDateYYYY);
    m_pqmenuAttributes->addAction(m_pqactFiCreatedDateYY);
    m_pqmenuAttributes->addAction(m_pqactFiCreatedDateMM);
    m_pqmenuAttributes->addAction(m_pqactFiCreatedDateDD);
    m_pqmenuAttributes->addAction(m_pqactFiCreatedTimeHH);
    m_pqmenuAttributes->addAction(m_pqactFiCreatedTimeMM);
    m_pqmenuAttributes->addAction(m_pqactFiCreatedTimeSS);
    m_pqmenuAttributes->addAction(m_pqactFiModifiedDateTime);
    m_pqmenuAttributes->addAction(m_pqactFiModifiedDate);
    m_pqmenuAttributes->addAction(m_pqactFiModifiedTime);
    m_pqmenuAttributes->addAction(m_pqactFiModifiedDateYYYY);
    m_pqmenuAttributes->addAction(m_pqactFiModifiedDateYY);
    m_pqmenuAttributes->addAction(m_pqactFiModifiedDateMM);
    m_pqmenuAttributes->addAction(m_pqactFiModifiedDateDD);
    m_pqmenuAttributes->addAction(m_pqactFiModifiedTimeHH);
    m_pqmenuAttributes->addAction(m_pqactFiModifiedTimeMM);
    m_pqmenuAttributes->addAction(m_pqactFiModifiedTimeSS);

    m_pqmenuRenames = m_pmwMainWindow->menuBar()->addMenu(tr("&Renames"));
    m_pqmenuRenames->addAction(m_pqactSaveCurrentSettings);
    m_pqmenuRenames->addAction(m_pqactOrganiseSavedRenames);
    m_pqmenuRenames->addSeparator();
    m_pqmenuRenames->addAction(m_pqactClearRenameSettings);
    m_pqmenuRenames->addSeparator();

    IComUIMenuBarBase::InitialiseMenuBar();
}


QString IUIMenuBar::PrintTagOS(const QString & krqstrTag)
{
    #ifdef Q_OS_MACOS
    return QString();
    #else
    return krqstrTag;
    #endif
}


void IUIMenuBar::AddNavigationActions(QToolBar* pqtpNavigationToolBar)
{
    pqtpNavigationToolBar->addAction(m_pqactNavigateBack);
    pqtpNavigationToolBar->addAction(m_pqactNavigateForward);
    pqtpNavigationToolBar->addAction(m_pqactNavigateUp);
    pqtpNavigationToolBar->addAction(m_pqactNavigateStartDir);
    pqtpNavigationToolBar->addAction(m_pqactNavigateRefresh);

}


void IUIMenuBar::AddPreferencesActions(QToolBar* pqtpNavigationToolBar)
{   
    pqtpNavigationToolBar->addAction(m_pqactShowHiddenFiles);
    pqtpNavigationToolBar->addAction(m_pqactCaseSensitiveCompare);
    pqtpNavigationToolBar->addAction(m_pqactPreferences);
}


void IUIMenuBar::AddSideBarActions(IUISideBar* puisbSideBar)
{
    puisbSideBar->addAction(m_pqactMoveSelectedLinesUp);
    puisbSideBar->addAction(m_pqactMoveSelectedLinesDown);
}


void IUIMenuBar::EnableBackAction(const bool kbEnable)
{
    m_pqactNavigateBack->setEnabled(kbEnable);
}


void IUIMenuBar::EnableForwardAction(const bool kbEnable)
{
    m_pqactNavigateForward->setEnabled(kbEnable);
}


void IUIMenuBar::EnableUpAction(const bool kbEnable)
{
    m_pqactNavigateUp->setEnabled(kbEnable);
}


bool IUIMenuBar::ShowHiddenFiles()
{
    return m_pqactShowHiddenFiles->isChecked();
}


bool IUIMenuBar::CaseSensitiveCompare()
{
    return m_pqactCaseSensitiveCompare->isChecked();
}


void IUIMenuBar::SyncSavedRenames(IUIRenameSettings* puirsRenameSettingsUI)
{
    QVector<QAction*>::iterator itRenameAction;
    for (itRenameAction = m_qvecpqactSavedRenames.begin() ; itRenameAction < m_qvecpqactSavedRenames.end() ; ++itRenameAction)
    {
        m_pqmenuRenames->removeAction(*itRenameAction);
        delete *itRenameAction;
    }
    m_qvecpqactSavedRenames.clear();

    const QVector<IRenameSave*> & krqvecpsrenSavedRenames = puirsRenameSettingsUI->GetSavedRenameSettings();

    QAction* pqactNewAction;
    QString qstrActionText;
    QVector<IRenameSave*>::const_iterator kitSavedRename;
    for (kitSavedRename = krqvecpsrenSavedRenames.constBegin() ; kitSavedRename < krqvecpsrenSavedRenames.constEnd() ; ++kitSavedRename)
    {
        qstrActionText = (*kitSavedRename)->m_qstrName;
        qstrActionText.replace('&', "&&");

        pqactNewAction = new QAction(qstrActionText, m_pqmenuFile->parent());
        pqactNewAction->setData(QVariant::fromValue(*kitSavedRename));
        m_pqmenuRenames->addAction(pqactNewAction);
        m_qvecpqactSavedRenames.push_back(pqactNewAction);
        QObject::connect(pqactNewAction, SIGNAL(triggered()), puirsRenameSettingsUI, SLOT(LoadSavedRanmeSetting()));
    }
}

void IUIMenuBar::ExecuteRename(const QString & krqstrName)
{
    QString qstrName = krqstrName.toLower();
    qstrName.replace('&', "&&");

    QVector<QAction*>::iterator itRenameAction;
    for (itRenameAction = m_qvecpqactSavedRenames.begin() ; itRenameAction < m_qvecpqactSavedRenames.end() ; ++itRenameAction)
    {
        if ((*itRenameAction)->text().toLower() == qstrName)
        {
            (*itRenameAction)->trigger();
            break;
        }
    }
}


QString IUIMenuBar::GetCreditsDependency()
{
    return tr("<a href=\"https://taglib.org/\">TagLib</a> - Used for reading music tags.<br>"
              "<a href=\"https://libexif.github.io/\">LibExif</a> - Used for reading Exif tags.<br>");
}



