#include <QtWidgets>
#include "IUIRenameSettings.h"
#include "IUIMainWindow.h"
#include "IUIMenuBar.h"
#include "IUIFileList.h"
#include "IRenameSave.h"
#include "IDlgSaveRenameSettings.h"
#include "IDlgOrganiseRenames.h"


IUIRenameSettings::IUIRenameSettings(IUIMainWindow* pmwMainWindow) : QWidget(pmwMainWindow),
                                                                     m_rpuifmFileList(pmwMainWindow->GetFileListUI()),
                                                                     m_rmtlMetaTagLookup(m_rpuifmFileList->GetMetaTagLookup())
{
    m_pmwMainWindow = pmwMainWindow;
    m_iRenameElements = RenameFilesOnly;
    m_pqleLastActiveLineEdit = nullptr;
    m_bClearingSettings = false;

    setupUi(this);
    m_pqgbNumberingType->resize(m_pqgbNumberingType->width(), m_pqgbNumberingType->sizeHint().height());
    m_pqgbNumberingZeroFill->resize(m_pqgbNumberingZeroFill->width(), m_pqgbNumberingType->sizeHint().height());
    m_pqgbNumberingIncrement->resize(m_pqgbNumberingIncrement->width(), m_pqgbNumberingType->sizeHint().height());

    layout()->setContentsMargins(0, 3, 0, 5);

    m_pqleRenameFilesWithExtension->setToolTip(tr("Enter extensions of file types you wish to rename, e.g. jpg mkv.\n"
                                                  "You can enter multiple file types by seperating them with spaces.\n"
                                                  "For example, if you wish to rename only image files you could enter:\n\n"
                                                  "\t\tjpg jpeg png gif bmp\n"));

    SetNumbersOnlyValidators();
    SetInvalidCharactersValidators();
    CreateNameSettingsConnections();
    CreateExtensionSettingsConnections();
    CreateNumberingSettingsConnections();
    CreateGlobalRenameSettingsConnections();
}


IUIRenameSettings::~IUIRenameSettings()
{
    SaveRenamesToQSettings();

    QVector<IRenameSave*>::const_iterator kitRenameSettings;
    for (kitRenameSettings = m_qvecpsrenSavedRenames.constBegin() ; kitRenameSettings != m_qvecpsrenSavedRenames.constEnd() ; ++kitRenameSettings)
        delete *kitRenameSettings;
    m_qvecpsrenSavedRenames.clear();
}


void IUIRenameSettings::SetNumbersOnlyValidators()
{
    m_pivalIntOnlyValidator = new QIntValidator(this);

    m_pqleNameInsertTheTextAtPos->setValidator(m_pivalIntOnlyValidator);
    m_pqleNameCropAtPos->setValidator(m_pivalIntOnlyValidator);
    m_pqleNameCropAtPosNextNChar->setValidator(m_pivalIntOnlyValidator);
    m_pqleNameLeftCropNChar->setValidator(m_pivalIntOnlyValidator);
    m_pqleNameRightCropNChar->setValidator(m_pivalIntOnlyValidator);

    m_pqleExtenInsertTheTextAtPos->setValidator(m_pivalIntOnlyValidator);
    m_pqleExtenCropAtPos->setValidator(m_pivalIntOnlyValidator);
    m_pqleExtenCropAtPosNextNChar->setValidator(m_pivalIntOnlyValidator);
    m_pqleExtenLeftCropNChar->setValidator(m_pivalIntOnlyValidator);
    m_pqleExtenRightCropNChar->setValidator(m_pivalIntOnlyValidator);

    m_pqleNumberingAtPos->setValidator(m_pivalIntOnlyValidator);
    m_pqleNumberingStartNum->setValidator(m_pivalIntOnlyValidator);
    m_pqleNumberingIncrement->setValidator(m_pivalIntOnlyValidator);
    m_pqleNumberingZeroFill->setValidator(m_pivalIntOnlyValidator);
}


void IUIRenameSettings::SetInvalidCharactersValidators()
{
    #ifdef Q_OS_WIN
    m_pqrevInvalidCharValidator = new QRegExpValidator(QRegExp("^[^\\\\/:*?\"<>|]*$"), this);     // Windows    \/:*?"<>|
    #else
    m_pqrevInvalidCharValidator = new QRegExpValidator(QRegExp("^[^/]*$"), this);                 // Linux+Mac  /
    #endif

    m_pqleNameReplaceName->setValidator(m_pqrevInvalidCharValidator);
    //m_pqleNameReplaceTheText->setValidator(m_pqrevInvalidCharValidator);
    m_pqleNameReplaceTheTextWith->setValidator(m_pqrevInvalidCharValidator);
    m_pqleNameInsertTheText->setValidator(m_pqrevInvalidCharValidator);
    m_pqleNameInsertAtStart->setValidator(m_pqrevInvalidCharValidator);
    m_pqleNameInsertAtEnd->setValidator(m_pqrevInvalidCharValidator);

    m_pqleExtenReplaceName->setValidator(m_pqrevInvalidCharValidator);
    //m_pqleExtenReplaceTheText->setValidator(m_pqrevInvalidCharValidator);
    m_pqleExtenReplaceTheTextWith->setValidator(m_pqrevInvalidCharValidator);
    m_pqleExtenInsertTheText->setValidator(m_pqrevInvalidCharValidator);
    m_pqleExtenInsertAtStart->setValidator(m_pqrevInvalidCharValidator);
    m_pqleExtenInsertAtEnd->setValidator(m_pqrevInvalidCharValidator);
}


void IUIRenameSettings::CreateNameSettingsConnections()
{
    connect(m_pqcbNameReplaceName,          SIGNAL(toggled(bool)),                  this,     SLOT(NameSettingsChangedReplaceName()));
    connect(m_pqleNameReplaceName,          SIGNAL(textChanged(const QString &)),   this,     SLOT(NameSettingsChangedReplaceName()));
    connect(m_pqleNameReplaceName,          SIGNAL(GotFocus()),                     this,     SLOT(StoreLastActiveLineEdit()));

    connect(m_pqcbNameReplaceTheText,       SIGNAL(toggled(bool)),                  this,     SLOT(NameSettingsChangedReplaceTheText()));
    connect(m_pqleNameReplaceTheText,       SIGNAL(textChanged(const QString &)),   this,     SLOT(NameSettingsChangedReplaceTheText()));
    connect(m_pqleNameReplaceTheTextWith,   SIGNAL(textChanged(const QString &)),   this,     SLOT(NameSettingsChangedReplaceTheText()));
    connect(m_pqleNameReplaceTheText,       SIGNAL(GotFocus()),                     this,     SLOT(ClearLastActiveLineEdit()));
    connect(m_pqleNameReplaceTheTextWith,   SIGNAL(GotFocus()),                     this,     SLOT(StoreLastActiveLineEdit()));

    connect(m_pqcbNameInsertTheText,        SIGNAL(toggled(bool)),                  this,     SLOT(NameSettingsChangedInsertTheText()));
    connect(m_pqleNameInsertTheText,        SIGNAL(textChanged(const QString &)),   this,     SLOT(NameSettingsChangedInsertTheText()));
    connect(m_pqleNameInsertTheTextAtPos,   SIGNAL(textChanged(const QString &)),   this,     SLOT(NameSettingsChangedInsertTheText()));
    connect(m_pqleNameInsertTheText,        SIGNAL(GotFocus()),                     this,     SLOT(StoreLastActiveLineEdit()));
    connect(m_pqleNameInsertTheTextAtPos,   SIGNAL(GotFocus()),                     this,     SLOT(ClearLastActiveLineEdit()));

    connect(m_pqcbNameInsertAtStart,        SIGNAL(toggled(bool)),                  this,     SLOT(NameSettingsChangedInsertAtStart()));
    connect(m_pqleNameInsertAtStart,        SIGNAL(textChanged(const QString &)),   this,     SLOT(NameSettingsChangedInsertAtStart()));
    connect(m_pqleNameInsertAtStart,        SIGNAL(GotFocus()),                     this,     SLOT(StoreLastActiveLineEdit()));

    connect(m_pqcbNameInsertAtEnd,          SIGNAL(toggled(bool)),                  this,     SLOT(NameSettingsChangedInsertAtEnd()));
    connect(m_pqleNameInsertAtEnd,          SIGNAL(textChanged(const QString &)),   this,     SLOT(NameSettingsChangedInsertAtEnd()));
    connect(m_pqleNameInsertAtEnd,          SIGNAL(GotFocus()),                     this,     SLOT(StoreLastActiveLineEdit()));

    connect(m_pqcbNameCropAtPos,            SIGNAL(toggled(bool)),                  this,     SLOT(NameSettingsChangedCropAtPos()));
    connect(m_pqleNameCropAtPos,            SIGNAL(textChanged(const QString &)),   this,     SLOT(NameSettingsChangedCropAtPos()));
    connect(m_pqleNameCropAtPosNextNChar,   SIGNAL(textChanged(const QString &)),   this,     SLOT(NameSettingsChangedCropAtPos()));
    connect(m_pqleNameCropAtPos,            SIGNAL(GotFocus()),                     this,     SLOT(ClearLastActiveLineEdit()));
    connect(m_pqleNameCropAtPosNextNChar,   SIGNAL(GotFocus()),                     this,     SLOT(ClearLastActiveLineEdit()));

    connect(m_pqcbNameLeftCropNChar,        SIGNAL(toggled(bool)),                  this,     SLOT(NameSettingsChangedLeftCrop()));
    connect(m_pqleNameLeftCropNChar,        SIGNAL(textChanged(const QString &)),   this,     SLOT(NameSettingsChangedLeftCrop()));
    connect(m_pqleNameLeftCropNChar,        SIGNAL(GotFocus()),                     this,     SLOT(ClearLastActiveLineEdit()));

    connect(m_pqcbNameRightCropNChar,       SIGNAL(toggled(bool)),                  this,     SLOT(NameSettingsChangedRightCrop()));
    connect(m_pqleNameRightCropNChar,       SIGNAL(textChanged(const QString &)),   this,     SLOT(NameSettingsChangedRightCrop()));
    connect(m_pqleNameRightCropNChar,       SIGNAL(GotFocus()),                     this,     SLOT(ClearLastActiveLineEdit()));

    connect(m_pqcboNameChangeCase,          SIGNAL(currentIndexChanged(int)),       this,     SLOT(NameSettingsChangedChangeCase()));
}


void IUIRenameSettings::CreateExtensionSettingsConnections()
{
    connect(m_pqcbExtenReplaceName,         SIGNAL(toggled(bool)),                  this,     SLOT(ExtensionSettingsChangedReplaceName()));
    connect(m_pqleExtenReplaceName,         SIGNAL(textChanged(const QString &)),   this,     SLOT(ExtensionSettingsChangedReplaceName()));
    connect(m_pqleExtenReplaceName,         SIGNAL(GotFocus()),                     this,     SLOT(StoreLastActiveLineEdit()));

    connect(m_pqcbExtenReplaceTheText,      SIGNAL(toggled(bool)),                  this,     SLOT(ExtensionSettingsChangedReplaceTheText()));
    connect(m_pqleExtenReplaceTheText,      SIGNAL(textChanged(const QString &)),   this,     SLOT(ExtensionSettingsChangedReplaceTheText()));
    connect(m_pqleExtenReplaceTheTextWith,  SIGNAL(textChanged(const QString &)),   this,     SLOT(ExtensionSettingsChangedReplaceTheText()));
    connect(m_pqleExtenReplaceTheText,      SIGNAL(GotFocus()),                     this,     SLOT(ClearLastActiveLineEdit()));
    connect(m_pqleExtenReplaceTheTextWith,  SIGNAL(GotFocus()),                     this,     SLOT(StoreLastActiveLineEdit()));

    connect(m_pqcbExtenInsertTheText,       SIGNAL(toggled(bool)),                  this,     SLOT(ExtensionSettingsChangedInsertTheText()));
    connect(m_pqleExtenInsertTheText,       SIGNAL(textChanged(const QString &)),   this,     SLOT(ExtensionSettingsChangedInsertTheText()));
    connect(m_pqleExtenInsertTheTextAtPos,  SIGNAL(textChanged(const QString &)),   this,     SLOT(ExtensionSettingsChangedInsertTheText()));
    connect(m_pqleExtenInsertTheText,       SIGNAL(GotFocus()),                     this,     SLOT(StoreLastActiveLineEdit()));
    connect(m_pqleExtenInsertTheTextAtPos,  SIGNAL(GotFocus()),                     this,     SLOT(ClearLastActiveLineEdit()));

    connect(m_pqcbExtenInsertAtStart,       SIGNAL(toggled(bool)),                  this,     SLOT(ExtensionSettingsChangedInsertAtStart()));
    connect(m_pqleExtenInsertAtStart,       SIGNAL(textChanged(const QString &)),   this,     SLOT(ExtensionSettingsChangedInsertAtStart()));
    connect(m_pqleExtenInsertAtStart,       SIGNAL(GotFocus()),                     this,     SLOT(StoreLastActiveLineEdit()));

    connect(m_pqcbExtenInsertAtEnd,         SIGNAL(toggled(bool)),                  this,     SLOT(ExtensionSettingsChangedInsertAtEnd()));
    connect(m_pqleExtenInsertAtEnd,         SIGNAL(textChanged(const QString &)),   this,     SLOT(ExtensionSettingsChangedInsertAtEnd()));
    connect(m_pqleExtenInsertAtEnd,         SIGNAL(GotFocus()),                     this,     SLOT(StoreLastActiveLineEdit()));

    connect(m_pqcbExtenCropAtPos,           SIGNAL(toggled(bool)),                  this,     SLOT(ExtensionSettingsChangedCropAtPos()));
    connect(m_pqleExtenCropAtPos,           SIGNAL(textChanged(const QString &)),   this,     SLOT(ExtensionSettingsChangedCropAtPos()));
    connect(m_pqleExtenCropAtPosNextNChar,  SIGNAL(textChanged(const QString &)),   this,     SLOT(ExtensionSettingsChangedCropAtPos()));
    connect(m_pqleExtenCropAtPos,           SIGNAL(GotFocus()),                     this,     SLOT(ClearLastActiveLineEdit()));
    connect(m_pqleExtenCropAtPosNextNChar,  SIGNAL(GotFocus()),                     this,     SLOT(ClearLastActiveLineEdit()));

    connect(m_pqcbExtenLeftCropNChar,       SIGNAL(toggled(bool)),                  this,     SLOT(ExtensionSettingsChangedLeftCrop()));
    connect(m_pqleExtenLeftCropNChar,       SIGNAL(textChanged(const QString &)),   this,     SLOT(ExtensionSettingsChangedLeftCrop()));
    connect(m_pqleExtenLeftCropNChar,       SIGNAL(GotFocus()),                     this,     SLOT(ClearLastActiveLineEdit()));

    connect(m_pqcbExtenRightCropNChar,      SIGNAL(toggled(bool)),                  this,     SLOT(ExtensionSettingsChangedRightCrop()));
    connect(m_pqleExtenRightCropNChar,      SIGNAL(textChanged(const QString &)),   this,     SLOT(ExtensionSettingsChangedRightCrop()));
    connect(m_pqleExtenRightCropNChar,      SIGNAL(GotFocus()),                     this,     SLOT(ClearLastActiveLineEdit()));

    connect(m_pqcboExtenChangeCase,         SIGNAL(currentIndexChanged(int)),       this,     SLOT(ExtensionSettingsChangedChangeCase()));
}


void IUIRenameSettings::CreateNumberingSettingsConnections()
{
    connect(m_pqrbNumberingNoNumber,        SIGNAL(toggled(bool)),                  this,     SLOT(NumberingRadioSettingsChanged(bool)));
    connect(m_pqrbNumberingBeforeName,      SIGNAL(toggled(bool)),                  this,     SLOT(NumberingRadioSettingsChanged(bool)));
    connect(m_pqrbNumberingAfterName,       SIGNAL(toggled(bool)),                  this,     SLOT(NumberingRadioSettingsChanged(bool)));
    connect(m_pqrbNumberingAtPos,           SIGNAL(toggled(bool)),                  this,     SLOT(NumberingRadioSettingsChanged(bool)));
    connect(m_pqleNumberingAtPos,           SIGNAL(textChanged(const QString &)),   this,     SLOT(NumberingSettingsChanged()));
    connect(m_pqleNumberingAtPos,           SIGNAL(GotFocus()),                     this,     SLOT(ClearLastActiveLineEdit()));

    connect(m_pqleNumberingStartNum,        SIGNAL(textChanged(const QString &)),   this,     SLOT(NumberingSettingsChanged()));
    connect(m_pqleNumberingIncrement,       SIGNAL(textChanged(const QString &)),   this,     SLOT(NumberingSettingsChanged()));
    connect(m_pqleNumberingZeroFill,        SIGNAL(textChanged(const QString &)),   this,     SLOT(NumberingSettingsChanged()));
    connect(m_pqrbNumberingZeroFillAuto,    SIGNAL(toggled(bool)),                  this,     SLOT(NumberingSettingsChanged()));
    connect(m_pqrbNumberingZeroFillSpec,    SIGNAL(toggled(bool)),                  this,     SLOT(NumberingSettingsChanged()));
    connect(m_pqleNumberingStartNum,        SIGNAL(GotFocus()),                     this,     SLOT(ClearLastActiveLineEdit()));
    connect(m_pqleNumberingIncrement,       SIGNAL(GotFocus()),                     this,     SLOT(ClearLastActiveLineEdit()));
    connect(m_pqleNumberingZeroFill,        SIGNAL(GotFocus()),                     this,     SLOT(ClearLastActiveLineEdit()));

    connect(m_pqrbNumberingNoNumber,        SIGNAL(toggled(bool)), m_pqleNumberingStartNum,   SLOT(setDisabled(bool)));
    connect(m_pqrbNumberingNoNumber,        SIGNAL(toggled(bool)), m_pqleNumberingIncrement,  SLOT(setDisabled(bool)));
    //connect(m_pqrbNumberingNoNumber,        SIGNAL(toggled(bool)),  m_pqleNumberingZeroFill,   SLOT(setDisabled(bool)));
    connect(m_pqrbNumberingNoNumber,        SIGNAL(toggled(bool)), m_pqrbNumberingZeroFillAuto,SLOT(setDisabled(bool)));
    connect(m_pqrbNumberingNoNumber,        SIGNAL(toggled(bool)), m_pqrbNumberingZeroFillSpec,SLOT(setDisabled(bool)));
}


void IUIRenameSettings::CreateGlobalRenameSettingsConnections()
{
     connect(m_pqrbRenameFilesOnly,          SIGNAL(toggled(bool)),                  this,     SLOT(RenameElementsRadioSettingsChanged(bool)));
     connect(m_pqrbRenameFoldersOnly,        SIGNAL(toggled(bool)),                  this,     SLOT(RenameElementsRadioSettingsChanged(bool)));
     connect(m_pqrbRenameFilesAndFolders,    SIGNAL(toggled(bool)),                  this,     SLOT(RenameElementsRadioSettingsChanged(bool)));
     connect(m_pqrbRenameSelectedItemsOnly,  SIGNAL(toggled(bool)),                  this,     SLOT(RenameElementsRadioSettingsChanged(bool)));
     connect(m_pqrbRenameFilesWithExtension, SIGNAL(toggled(bool)),                  this,     SLOT(RenameElementsRadioSettingsChanged(bool)));

     connect(m_pqleRenameFilesWithExtension, SIGNAL(textChanged(const QString &)),   this,     SLOT(RenameElementsExtensionChanged()));
     connect(m_pqleRenameFilesWithExtension, SIGNAL(GotFocus()),                     this,     SLOT(ClearLastActiveLineEdit()));

     connect(m_pqpbRenameButton,             SIGNAL(clicked()),                      this,     SLOT(ConfirmRename()));
     connect(m_pqpbUndoButton,               SIGNAL(clicked()),                      this,     SLOT(ConfirmUndo()));
}


void IUIRenameSettings::DisableAllSettings()
{
    m_bClearingSettings = true;

    m_pqcbNameReplaceName->setChecked(false);
    m_pqcbNameReplaceTheText->setChecked(false);
    m_pqcbNameInsertTheText->setChecked(false);
    m_pqcbNameInsertAtStart->setChecked(false);
    m_pqcbNameInsertAtEnd->setChecked(false);
    m_pqcbNameCropAtPos->setChecked(false);
    m_pqcbNameLeftCropNChar->setChecked(false);
    m_pqcbNameRightCropNChar->setChecked(false);
    m_pqcboNameChangeCase->setCurrentIndex(0);

    m_pqcbExtenReplaceName->setChecked(false);
    m_pqcbExtenReplaceTheText->setChecked(false);
    m_pqcbExtenInsertTheText->setChecked(false);
    m_pqcbExtenInsertAtStart->setChecked(false);
    m_pqcbExtenInsertAtEnd->setChecked(false);
    m_pqcbExtenCropAtPos->setChecked(false);
    m_pqcbExtenLeftCropNChar->setChecked(false);
    m_pqcbExtenRightCropNChar->setChecked(false);
    m_pqcboExtenChangeCase->setCurrentIndex(0);

    m_pqrbNumberingNoNumber->setChecked(true);

    m_bClearingSettings = false;
    m_rpuifmFileList->GeneratePreviewNameAndExtension();
}


void IUIRenameSettings::ClearAll()
{
    m_bClearingSettings = true;

    m_pqcbNameReplaceName->setChecked(false);
    m_pqcbNameReplaceTheText->setChecked(false);
    m_pqcbNameInsertTheText->setChecked(false);
    m_pqcbNameInsertAtStart->setChecked(false);
    m_pqcbNameInsertAtEnd->setChecked(false);
    m_pqcbNameCropAtPos->setChecked(false);
    m_pqcbNameLeftCropNChar->setChecked(false);
    m_pqcbNameRightCropNChar->setChecked(false);
    m_pqcboNameChangeCase->setCurrentIndex(0);

    m_pqcbExtenReplaceName->setChecked(false);
    m_pqcbExtenReplaceTheText->setChecked(false);
    m_pqcbExtenInsertTheText->setChecked(false);
    m_pqcbExtenInsertAtStart->setChecked(false);
    m_pqcbExtenInsertAtEnd->setChecked(false);
    m_pqcbExtenCropAtPos->setChecked(false);
    m_pqcbExtenLeftCropNChar->setChecked(false);
    m_pqcbExtenRightCropNChar->setChecked(false);
    m_pqcboExtenChangeCase->setCurrentIndex(0);

    m_pqleNameReplaceName->clear();
    m_pqleNameReplaceTheText->clear();
    m_pqleNameReplaceTheTextWith->clear();
    m_pqleNameInsertTheText->clear();
    m_pqleNameInsertTheTextAtPos->clear();
    m_pqleNameInsertAtStart->clear();
    m_pqleNameInsertAtEnd->clear();
    m_pqleNameCropAtPos->clear();
    m_pqleNameCropAtPosNextNChar->clear();
    m_pqleNameLeftCropNChar->clear();
    m_pqleNameRightCropNChar->clear();

    m_pqleExtenReplaceName->clear();
    m_pqleExtenReplaceTheText->clear();
    m_pqleExtenReplaceTheTextWith->clear();
    m_pqleExtenInsertTheText->clear();
    m_pqleExtenInsertTheTextAtPos->clear();
    m_pqleExtenInsertAtStart->clear();
    m_pqleExtenInsertAtEnd->clear();
    m_pqleExtenCropAtPos->clear();
    m_pqleExtenCropAtPosNextNChar->clear();
    m_pqleExtenLeftCropNChar->clear();
    m_pqleExtenRightCropNChar->clear();

    m_pqrbNumberingNoNumber->setChecked(true);
    m_pqleNumberingAtPos->clear();
    m_pqleNumberingStartNum->setText("1");
    m_pqleNumberingIncrement->setText("1");

    m_pqrbNumberingZeroFillAuto->setChecked(true);
    m_pqleNumberingZeroFill->setText("1");

    m_pqrbRenameFilesOnly->setChecked(true);
    m_pqleRenameFilesWithExtension->clear();

    m_bClearingSettings = false;
    m_rpuifmFileList->GeneratePreviewNameAndExtension();
}


void IUIRenameSettings::StoreLastActiveLineEdit()
{
    m_pqleLastActiveLineEdit = qobject_cast<QLineEdit*>(sender());
}


void IUIRenameSettings::ClearLastActiveLineEdit()
{
    m_pqleLastActiveLineEdit = nullptr;
}


void IUIRenameSettings::AddActionLookupMusic(QAction* qactAction, const QString & qstrTagCode)
{
    m_qhashActionLookupMusic[qactAction] = qstrTagCode;
    QObject::connect(qactAction, SIGNAL(triggered()), this, SLOT(InsertTagCodeMusic()));
}


void IUIRenameSettings::AddActionLookupExif(QAction* qactAction, const QString & qstrTagCode)
{
    m_qhashActionLookupExif[qactAction] = qstrTagCode;
    QObject::connect(qactAction, SIGNAL(triggered()), this, SLOT(InsertTagCodeExif()));
}


void IUIRenameSettings::AddActionLookupFiAt(QAction* qactAction, const QString & qstrTagCode)
{
    m_qhashActionLookupFiAt[qactAction] = qstrTagCode;
    QObject::connect(qactAction, SIGNAL(triggered()), this, SLOT(InsertTagCodeFiAt()));
}


void IUIRenameSettings::InsertTagCodeMusic()
{
    if (m_pqleLastActiveLineEdit != nullptr && m_pqleLastActiveLineEdit->isEnabled())
    {
        int iCursorPos = m_pqleLastActiveLineEdit->cursorPosition();
        QString qstrText = m_pqleLastActiveLineEdit->text();
        QString qstrTagCode = m_qhashActionLookupMusic[qobject_cast<QAction*>(sender())];
        qstrText.insert(iCursorPos, qstrTagCode);
        m_pqleLastActiveLineEdit->setText(qstrText);
        m_pqleLastActiveLineEdit->setCursorPosition(iCursorPos + qstrTagCode.length());
    }
}


void IUIRenameSettings::InsertTagCodeExif()
{
    if (m_pqleLastActiveLineEdit != nullptr && m_pqleLastActiveLineEdit->isEnabled())
    {
        int iCursorPos = m_pqleLastActiveLineEdit->cursorPosition();
        QString qstrText = m_pqleLastActiveLineEdit->text();
        QString qstrTagCode = m_qhashActionLookupExif[qobject_cast<QAction*>(sender())];
        qstrText.insert(iCursorPos, qstrTagCode);
        m_pqleLastActiveLineEdit->setText(qstrText);
        m_pqleLastActiveLineEdit->setCursorPosition(iCursorPos + qstrTagCode.length());
    }
}


void IUIRenameSettings::InsertTagCodeFiAt()
{
    if (m_pqleLastActiveLineEdit != nullptr && m_pqleLastActiveLineEdit->isEnabled())
    {
        int iCursorPos = m_pqleLastActiveLineEdit->cursorPosition();
        QString qstrText = m_pqleLastActiveLineEdit->text();
        QString qstrTagCode = m_qhashActionLookupFiAt[qobject_cast<QAction*>(sender())];
        qstrText.insert(iCursorPos, qstrTagCode);
        m_pqleLastActiveLineEdit->setText(qstrText);
        m_pqleLastActiveLineEdit->setCursorPosition(iCursorPos + qstrTagCode.length());
    }
}


void IUIRenameSettings::ReadTagCodes(const QString & krqstrString, QList<ITagInfo> & rqlstTagList)
{
    rqlstTagList.clear();

    if (m_bClearingSettings)
        return;

    int iSearchStart = 0;
    ITagInfo tagiTagInfo;
    while (true)
    {
        tagiTagInfo.m_iStartIndex = krqstrString.indexOf("[$", iSearchStart);
        if (tagiTagInfo.m_iStartIndex == -1)
            break;
        int iCatIndex = tagiTagInfo.m_iStartIndex + 2;

        int iLength = krqstrString.length();
        int iTagIndex = iCatIndex;
        while (iTagIndex < iLength && krqstrString.at(iTagIndex) != '-')
            ++iTagIndex;

        if (iTagIndex >= iLength)
            break;
        ++iTagIndex;

        tagiTagInfo.m_iEndIndex = iTagIndex;
        while (tagiTagInfo.m_iEndIndex < iLength && krqstrString.at(tagiTagInfo.m_iEndIndex) != ']')
            ++tagiTagInfo.m_iEndIndex;

        if (tagiTagInfo.m_iEndIndex >= iLength)
            break;

        tagiTagInfo.m_iTagID = IMetaTagLookup::InvalidTag;
        QString qstrCat = krqstrString.mid(iCatIndex, iTagIndex - iCatIndex - 1).toLower();
        QString qstrTag = krqstrString.mid(iTagIndex, tagiTagInfo.m_iEndIndex - iTagIndex).toLower();

        if      (qstrCat == "mu")
            tagiTagInfo.m_iTagID = m_rmtlMetaTagLookup.m_qhashTagIdLookupMusic[qstrTag];
        else if (qstrCat == "ex")
            tagiTagInfo.m_iTagID = m_rmtlMetaTagLookup.m_qhashTagIdLookupExif[qstrTag];
        else if (qstrCat == "at")
            tagiTagInfo.m_iTagID = m_rmtlMetaTagLookup.m_qhashTagIdLookupFiAt[qstrTag];

        if (tagiTagInfo.m_iTagID != IMetaTagLookup::InvalidTag)
            rqlstTagList.push_back(tagiTagInfo);

        iSearchStart = tagiTagInfo.m_iEndIndex + 1;
    }
}


void IUIRenameSettings::NameSettingsChangedReplaceName()
{
    m_rnamNameSettings.m_bReplaceName             = m_pqcbNameReplaceName->isChecked();
    m_rnamNameSettings.m_qstrReplaceName          = m_pqleNameReplaceName->text();

    if (m_rnamNameSettings.m_bReplaceName)
        ReadTagCodes(m_rnamNameSettings.m_qstrReplaceName, m_rnamNameSettings.m_qlstReplaceNameTags);
    else
        m_rnamNameSettings.m_qlstReplaceNameTags.clear();

    m_rpuifmFileList->GeneratePreviewName();
}


void IUIRenameSettings::NameSettingsChangedReplaceTheText()
{
    m_rnamNameSettings.m_bReplaceTheText          = m_pqcbNameReplaceTheText->isChecked() && m_pqleNameReplaceTheText->text().isEmpty() == false;
    m_rnamNameSettings.m_qstrReplaceTheText       = m_pqleNameReplaceTheText->text();
    m_rnamNameSettings.m_qstrReplaceTheTextWith   = m_pqleNameReplaceTheTextWith->text();

    if (m_rnamNameSettings.m_bReplaceTheText)
        ReadTagCodes(m_rnamNameSettings.m_qstrReplaceTheTextWith, m_rnamNameSettings.m_qlstReplaceTheTextWithTags);
    else
        m_rnamNameSettings.m_qlstReplaceTheTextWithTags.clear();

    m_rpuifmFileList->GeneratePreviewName();
}


void IUIRenameSettings::NameSettingsChangedInsertTheText()
{
    m_rnamNameSettings.m_bInsertTheText           = m_pqcbNameInsertTheText->isChecked() && m_pqleNameInsertTheTextAtPos->text().isEmpty() == false;
    m_rnamNameSettings.m_qstrInsertTheText        = m_pqleNameInsertTheText->text();
    m_rnamNameSettings.m_iInsertTheTextAtPos      = m_pqleNameInsertTheTextAtPos->text().toInt();

    if (m_rnamNameSettings.m_bInsertTheText)
        ReadTagCodes(m_rnamNameSettings.m_qstrInsertTheText, m_rnamNameSettings.m_qlstInsertTheTextTags);
    else
        m_rnamNameSettings.m_qlstInsertTheTextTags.clear();

    m_rpuifmFileList->GeneratePreviewName();
}


void IUIRenameSettings::NameSettingsChangedInsertAtStart()
{
    m_rnamNameSettings.m_bInsertAtStart           = m_pqcbNameInsertAtStart->isChecked();
    m_rnamNameSettings.m_qstrInsertAtStart        = m_pqleNameInsertAtStart->text();

    if (m_rnamNameSettings.m_bInsertAtStart)
        ReadTagCodes(m_rnamNameSettings.m_qstrInsertAtStart, m_rnamNameSettings.m_qlstInsertAtStartTags);
    else
        m_rnamNameSettings.m_qlstInsertAtStartTags.clear();

    m_rpuifmFileList->GeneratePreviewName();
}


void IUIRenameSettings::NameSettingsChangedInsertAtEnd()
{
    m_rnamNameSettings.m_bInsertAtEnd             = m_pqcbNameInsertAtEnd->isChecked();
    m_rnamNameSettings.m_qstrInsertAtEnd          = m_pqleNameInsertAtEnd->text();

    if (m_rnamNameSettings.m_bInsertAtEnd)
        ReadTagCodes(m_rnamNameSettings.m_qstrInsertAtEnd, m_rnamNameSettings.m_qlstInsertAtEndTags);
    else
        m_rnamNameSettings.m_qlstInsertAtEndTags.clear();

    m_rpuifmFileList->GeneratePreviewName();
}


void IUIRenameSettings::NameSettingsChangedCropAtPos()
{
    m_rnamNameSettings.m_bCropAtPos               = m_pqcbNameCropAtPos->isChecked() && m_pqleNameCropAtPos->text().isEmpty() == false;
    m_rnamNameSettings.m_iCropAtPos               = m_pqleNameCropAtPos->text().toInt();
    m_rnamNameSettings.m_iCropAtPosNextNChar      = m_pqleNameCropAtPosNextNChar->text().toInt();

    m_rpuifmFileList->GeneratePreviewName();
}

void IUIRenameSettings::NameSettingsChangedLeftCrop()
{
    m_rnamNameSettings.m_bLeftCropNChar           = m_pqcbNameLeftCropNChar->isChecked();
    m_rnamNameSettings.m_iLeftCropNChar           = m_pqleNameLeftCropNChar->text().toInt();

    m_rpuifmFileList->GeneratePreviewName();
}

void IUIRenameSettings::NameSettingsChangedRightCrop()
{
    m_rnamNameSettings.m_bRightCropNChar          = m_pqcbNameRightCropNChar->isChecked();
    m_rnamNameSettings.m_iRightCropNChar          = m_pqleNameRightCropNChar->text().toInt();

    m_rpuifmFileList->GeneratePreviewName();
}

void IUIRenameSettings::NameSettingsChangedChangeCase()
{
    m_rnamNameSettings.m_iChangeCase              = m_pqcboNameChangeCase->currentIndex();

    m_rpuifmFileList->GeneratePreviewName();
}


void IUIRenameSettings::ExtensionSettingsChangedReplaceName()
{
    m_rnamExtenSettings.m_bReplaceName            = m_pqcbExtenReplaceName->isChecked();
    m_rnamExtenSettings.m_qstrReplaceName         = m_pqleExtenReplaceName->text();

    if (m_rnamExtenSettings.m_bReplaceName)
        ReadTagCodes(m_rnamExtenSettings.m_qstrReplaceName, m_rnamExtenSettings.m_qlstReplaceNameTags);
    else
        m_rnamExtenSettings.m_qlstReplaceNameTags.clear();

    m_rpuifmFileList->GeneratePreviewExtension();
}


void IUIRenameSettings::ExtensionSettingsChangedReplaceTheText()
{
    m_rnamExtenSettings.m_bReplaceTheText         = m_pqcbExtenReplaceTheText->isChecked() && m_pqleExtenReplaceTheText->text().isEmpty() == false;
    m_rnamExtenSettings.m_qstrReplaceTheText      = m_pqleExtenReplaceTheText->text();
    m_rnamExtenSettings.m_qstrReplaceTheTextWith  = m_pqleExtenReplaceTheTextWith->text();

    if (m_rnamExtenSettings.m_bReplaceTheText)
        ReadTagCodes(m_rnamExtenSettings.m_qstrReplaceTheTextWith, m_rnamExtenSettings.m_qlstReplaceTheTextWithTags);
    else
        m_rnamExtenSettings.m_qlstReplaceTheTextWithTags.clear();

    m_rpuifmFileList->GeneratePreviewExtension();
}

void IUIRenameSettings::ExtensionSettingsChangedInsertTheText()
{
    m_rnamExtenSettings.m_bInsertTheText          = m_pqcbExtenInsertTheText->isChecked() && m_pqleExtenInsertTheTextAtPos->text().isEmpty() == false;
    m_rnamExtenSettings.m_qstrInsertTheText       = m_pqleExtenInsertTheText->text();
    m_rnamExtenSettings.m_iInsertTheTextAtPos     = m_pqleExtenInsertTheTextAtPos->text().toInt();

    if (m_rnamExtenSettings.m_bInsertTheText)
        ReadTagCodes(m_rnamExtenSettings.m_qstrInsertTheText, m_rnamExtenSettings.m_qlstInsertTheTextTags);
    else
        m_rnamExtenSettings.m_qlstInsertTheTextTags.clear();

    m_rpuifmFileList->GeneratePreviewExtension();
}

void IUIRenameSettings::ExtensionSettingsChangedInsertAtStart()
{
    m_rnamExtenSettings.m_bInsertAtStart          = m_pqcbExtenInsertAtStart->isChecked();
    m_rnamExtenSettings.m_qstrInsertAtStart       = m_pqleExtenInsertAtStart->text();

    if (m_rnamExtenSettings.m_bInsertAtStart)
        ReadTagCodes(m_rnamExtenSettings.m_qstrInsertAtStart, m_rnamExtenSettings.m_qlstInsertAtStartTags);
    else
        m_rnamExtenSettings.m_qlstInsertAtStartTags.clear();

    m_rpuifmFileList->GeneratePreviewExtension();
}

void IUIRenameSettings::ExtensionSettingsChangedInsertAtEnd()
{
    m_rnamExtenSettings.m_bInsertAtEnd            = m_pqcbExtenInsertAtEnd->isChecked();
    m_rnamExtenSettings.m_qstrInsertAtEnd         = m_pqleExtenInsertAtEnd->text();

    if (m_rnamExtenSettings.m_bInsertAtEnd)
        ReadTagCodes(m_rnamExtenSettings.m_qstrInsertAtEnd, m_rnamExtenSettings.m_qlstInsertAtEndTags);
    else
        m_rnamExtenSettings.m_qlstInsertAtEndTags.clear();

    m_rpuifmFileList->GeneratePreviewExtension();
}

void IUIRenameSettings::ExtensionSettingsChangedCropAtPos()
{
    m_rnamExtenSettings.m_bCropAtPos              = m_pqcbExtenCropAtPos->isChecked() && m_pqleExtenCropAtPos->text().isEmpty() == false;
    m_rnamExtenSettings.m_iCropAtPos              = m_pqleExtenCropAtPos->text().toInt();
    m_rnamExtenSettings.m_iCropAtPosNextNChar     = m_pqleExtenCropAtPosNextNChar->text().toInt();

    m_rpuifmFileList->GeneratePreviewExtension();
}

void IUIRenameSettings::ExtensionSettingsChangedLeftCrop()
{
    m_rnamExtenSettings.m_bLeftCropNChar          = m_pqcbExtenLeftCropNChar->isChecked();
    m_rnamExtenSettings.m_iLeftCropNChar          = m_pqleExtenLeftCropNChar->text().toInt();

    m_rpuifmFileList->GeneratePreviewExtension();
}

void IUIRenameSettings::ExtensionSettingsChangedRightCrop()
{
    m_rnamExtenSettings.m_bRightCropNChar         = m_pqcbExtenRightCropNChar->isChecked();
    m_rnamExtenSettings.m_iRightCropNChar         = m_pqleExtenRightCropNChar->text().toInt();

    m_rpuifmFileList->GeneratePreviewExtension();
}

void IUIRenameSettings::ExtensionSettingsChangedChangeCase()
{
    m_rnamExtenSettings.m_iChangeCase             = m_pqcboExtenChangeCase->currentIndex();

    m_rpuifmFileList->GeneratePreviewExtension();
}


void IUIRenameSettings::NumberingRadioSettingsChanged(const bool kbEnabled)
{
    if (kbEnabled == false)
        return;

    int iPrevSetting = m_rnumNumberingSettings.m_iNumberingType;
    if (m_pqrbNumberingNoNumber->isChecked())
        m_rnumNumberingSettings.m_iNumberingType = IRenameNumber::NumberNoNumbering;
    else if (m_pqrbNumberingAfterName->isChecked())
        m_rnumNumberingSettings.m_iNumberingType = IRenameNumber::NumberAfterName;
    else if (m_pqrbNumberingBeforeName->isChecked())
        m_rnumNumberingSettings.m_iNumberingType = IRenameNumber::NumberBeforeName;
    else if (m_pqrbNumberingAtPos->isChecked())
        m_rnumNumberingSettings.m_iNumberingType = IRenameNumber::NumberAtPos;

    if (iPrevSetting != m_rnumNumberingSettings.m_iNumberingType)
        NumberingSettingsChanged();
}


void IUIRenameSettings::NumberingSettingsChanged()
{
    if (m_rnumNumberingSettings.m_iNumberingType != IRenameNumber::NumberNoNumbering)
    {
        if (m_rnumNumberingSettings.m_iNumberingType == IRenameNumber::NumberAtPos)
            m_rnumNumberingSettings.m_iNumberingAtPos = m_pqleNumberingAtPos->text().toInt();

        m_rnumNumberingSettings.m_iNumberingStartNum    = m_pqleNumberingStartNum->text().toInt();
        m_rnumNumberingSettings.m_iNumberingIncrement   = m_pqleNumberingIncrement->text().toInt();
        m_rnumNumberingSettings.m_iNumberingZeroFill    = m_pqleNumberingZeroFill->text().toInt();
        m_rnumNumberingSettings.m_bNumberingZeroFillAuto= m_pqrbNumberingZeroFillAuto->isChecked();
    }

    m_rpuifmFileList->GeneratePreviewName();
}


void IUIRenameSettings::RenameElementsRadioSettingsChanged(const bool kbEnabled)
{
    if (kbEnabled == false)
        return;

    int iPrevSetting = m_iRenameElements;
    if (m_pqrbRenameFilesOnly->isChecked())
        m_iRenameElements = RenameFilesOnly;
    else if (m_pqrbRenameFoldersOnly->isChecked())
        m_iRenameElements = RenameFoldersOnly;
    else if (m_pqrbRenameFilesAndFolders->isChecked())
        m_iRenameElements = RenameFilesAndFolders;
    else if (m_pqrbRenameSelectedItemsOnly->isChecked())
        m_iRenameElements = RenameSelectedItems;
    else //if (m_pqrbRenameFilesWithExtension->isChecked())
        m_iRenameElements = RenameFilesWithExtension;

    if (m_iRenameElements != iPrevSetting)
        m_rpuifmFileList->RenameElementsSettingsChanged();
}


void IUIRenameSettings::RenameElementsExtensionChanged()
{
    ReadRenameExtensions(m_rpuifmFileList->GetCaseSensitiveCompare());
    m_rpuifmFileList->RenameElementsSettingsChanged();
}


void IUIRenameSettings::ReadRenameExtensions(const bool kbCaseSensitive)
{
    QString qstrExtensions = m_pqleRenameFilesWithExtension->text();

    qstrExtensions.remove(QChar('*'));
    qstrExtensions.remove(QChar('.'));
    qstrExtensions.remove(QChar(','));

    if (kbCaseSensitive == false)
        qstrExtensions = qstrExtensions.toLower();

    m_qstrlRenameExtensions = qstrExtensions.split(QChar(' '), QString::SkipEmptyParts);
}


void IUIRenameSettings::ConfirmRename()
{
    int iResponse = QMessageBox::Yes;
    if (m_rpuifmFileList->ShowConfirmBeforeRename())
        iResponse = QMessageBox::question(this, tr("Continue With Rename?"),tr("Do you wish to proceed with the rename operation?"), QMessageBox::Yes | QMessageBox::No);

    if (iResponse == QMessageBox::Yes)
    {
        m_rpuifmFileList->PerformRename();

        if (m_rpuifmFileList->DeactivateSettingsAfterRename())
            DisableAllSettings();
    }
}


void IUIRenameSettings::ConfirmUndo()
{
    int iResponse = QMessageBox::Yes;
    if (m_rpuifmFileList->ShowConfirmBeforeRename())
        iResponse = QMessageBox::question(this, tr("Continue With Undo?"),tr("Do you wish to undo the last rename operation?"), QMessageBox::Yes | QMessageBox::No);

    if (iResponse == QMessageBox::Yes)
    {
        m_rpuifmFileList->UndoRename();

        if (m_rpuifmFileList->DeactivateSettingsAfterRename())
            DisableAllSettings();
    }
}


void IUIRenameSettings::ShowSaveRenameSettingsDialog()
{
    new IDlgSaveRenameSettings(this);
}


void IUIRenameSettings::ShowOrganiseSavedRenamesDialog()
{
    new IDlgOrganiseRenames(this);
}


void IUIRenameSettings::LoadSavedRanmeSetting()
{
    qobject_cast<QAction*>(sender())->data().value<IRenameSave*>()->RestoreSettings(this);
}


void IUIRenameSettings::SaveRenameSettings(const QString & krqstrName, IRenameSave* psrsOverwriteSettings)
{
    IRenameSave* psrsRenameSettings;
    if (psrsOverwriteSettings != nullptr)
    {
        psrsRenameSettings = psrsOverwriteSettings;
    }
    else
    {
        psrsRenameSettings = new IRenameSave();
        m_qvecpsrenSavedRenames.push_back(psrsRenameSettings);
    }

    psrsRenameSettings->SaveCurrentSettings(krqstrName, this);
    m_pmwMainWindow->GetMenuBar()->SyncSavedRenames(this);
}


void IUIRenameSettings::LoadRenamesFromQSettings()
{
    QSettings & rqsetSettings = m_pmwMainWindow->GetSettings();

    rqsetSettings.beginGroup("SavedRenames");
    IRenameSave* psrsRenameSettings;
    int iNumElements = rqsetSettings.beginReadArray("RenameSaves");
    for (int iArrayIndex = 0 ; iArrayIndex < iNumElements ; ++iArrayIndex)
    {
        rqsetSettings.setArrayIndex(iArrayIndex);
        psrsRenameSettings = new IRenameSave;
        psrsRenameSettings->ReadFromString(rqsetSettings.value("RenameSettings").toString());
        m_qvecpsrenSavedRenames.push_back(psrsRenameSettings);
    }
    rqsetSettings.endArray();
    rqsetSettings.endGroup();

    m_pmwMainWindow->GetMenuBar()->SyncSavedRenames(this);
}


void IUIRenameSettings::SaveRenamesToQSettings()
{
    QSettings & rqsetSettings = m_pmwMainWindow->GetSettings();
    rqsetSettings.remove("SavedRenames");

    rqsetSettings.beginGroup("SavedRenames");
    rqsetSettings.beginWriteArray("RenameSaves");
    int iArrayIndex = 0;
    QVector<IRenameSave*>::ConstIterator kitSavedRename;
    for (kitSavedRename = m_qvecpsrenSavedRenames.constBegin() ; kitSavedRename < m_qvecpsrenSavedRenames.constEnd() ; ++kitSavedRename, ++iArrayIndex)
    {
        rqsetSettings.setArrayIndex(iArrayIndex);
        rqsetSettings.setValue("RenameSettings", (*kitSavedRename)->OutputToString());
    }
    rqsetSettings.endArray();
    rqsetSettings.endGroup();

    rqsetSettings.sync();
}


IRenameSave* IUIRenameSettings::GetRenameSettingsFromName(const QString & krqstrName)
{
    QVector<IRenameSave*>::const_iterator kitRenameSettings;
    for (kitRenameSettings = m_qvecpsrenSavedRenames.constBegin() ; kitRenameSettings != m_qvecpsrenSavedRenames.constEnd() ; ++kitRenameSettings)
    {
        if ((*kitRenameSettings)->m_qstrName == krqstrName)
            return *kitRenameSettings;
    }
    return nullptr;
}


void IUIRenameSettings::ReadRenamesFromListWidget(const QListWidget* kpqlwRenameList)
{
    m_qvecpsrenSavedRenames.clear();

    QListWidgetItem* pqlwiItem;
    int iNumRows = kpqlwRenameList->count();
    for (int iRow = 0 ; iRow < iNumRows ; ++iRow)
    {
        pqlwiItem = kpqlwRenameList->item(iRow);
        m_qvecpsrenSavedRenames.push_back(pqlwiItem->data(Qt::UserRole).value<IRenameSave*>());
    }

    m_pmwMainWindow->GetMenuBar()->SyncSavedRenames(this);
 }
