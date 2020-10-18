QT += core gui widgets network
TEMPLATE = app

VERSION = 7.0
NAME = "Inviska Rename"
NAME_NO_SPACES = "InviskaRename"

INCLUDEPATH += ../Common ../../Libs/TagLib/include/taglib ../../Libs/LibExif
DEPENDPATH  += ../Common ../../Libs/TagLib/include/taglib ../../Libs/LibExif

LIBS += \
    $$PWD/../../Libs/TagLib/lib/libtag.a \
    $$PWD/../../Libs/LibExif/libexif/.libs/libexif.a

DEFINES += TAGLIB_STATIC

HEADERS += \
    ../Common/IComDlgCredits.h \
    ../Common/IComDlgFileProperties.h \
    ../Common/IComDlgHelpAbout.h \
    ../Common/IComDlgProgress.h \
    ../Common/IComMetaExif.h \
    ../Common/IComMetaMusic.h \
    ../Common/IComQLineEdit.h \
    ../Common/IComSysAbsoluteDay.h \
    ../Common/IComSysLatestVersion.h \
    ../Common/IComSysSingleInstance.h \
    ../Common/IComSysSorts.h \
    ../Common/IComUIMainWinBase.h \
    ../Common/IComUIMenuBarBase.h \
    ../Common/IComUIPrefGeneral.h \
    ../Common/IComUtilityFuncs.h \
    ../Common/IComWdgtMetaExif.h \
    ../Common/IComWdgtMetaMusic.h \
    IDlgOrganiseRenames.h \
    IDlgPreferences.h \
    IDlgRenameErrorList.h \
    IDlgRenameFile.h \
    IDlgSaveRenameSettings.h \
    IMetaExif.h \
    IMetaMusic.h \
    IMetaTagLookup.h \
    IRenameInvalidCharSub.h \
    IRenameName.h \
    IRenameNumber.h \
    IRenameSave.h \
    ISysFileInfoSort.h \
    IUIFileList.h \
    IUIMainWindow.h \
    IUIMenuBar.h \
    IUISideBar.h \
    IUIToolBar.h \
    IUIRenameSettings.h

SOURCES += \
    ../Common/IComDlgCredits.cpp \
    ../Common/IComDlgFileProperties.cpp \
    ../Common/IComDlgHelpAbout.cpp \
    ../Common/IComDlgProgress.cpp \
    ../Common/IComMetaExif.cpp \
    ../Common/IComMetaMusic.cpp \
    ../Common/IComQLineEdit.cpp \
    ../Common/IComSysAbsoluteDay.cpp \
    ../Common/IComSysLatestVersion.cpp \
    ../Common/IComSysSingleInstance.cpp \
    ../Common/IComUIMainWinBase.cpp \
    ../Common/IComUIMenuBarBase.cpp \
    ../Common/IComUIPrefGeneral.cpp \
    ../Common/IComUtilityFuncs.cpp \
    ../Common/IComWdgtMetaExif.cpp \
    ../Common/IComWdgtMetaMusic.cpp \
    ../Common/InviskaMain.cpp \
    IDlgOrganiseRenames.cpp \
    IDlgPreferences.cpp \
    IDlgRenameErrorList.cpp \
    IDlgRenameFile.cpp \
    IDlgSaveRenameSettings.cpp \
    IMetaExif.cpp \
    IMetaMusic.cpp \
    IMetaTagLookup.cpp \
    IRenameInvalidCharSub.cpp \
    IRenameName.cpp \
    IRenameNumber.cpp \
    IRenameSave.cpp \
    ISysFileInfoSort.cpp \
    IUIFileList.cpp \
    IUIMainWindow.cpp \
    IUIMenuBar.cpp \
    IUIRenameSettings.cpp \
    IUISideBar.cpp \
    IUIToolBar.cpp

FORMS += \
    ../Common/UIComPrefGeneral.ui \
    UIPreferences.ui \
    UIRenameSettings.ui

RESOURCES += \
    Resources.qrc

DEFINES += \
    QT_DEPRECATED_WARNINGS \
    APP_VERSION=\"\\\"$${VERSION}\\\"\" \
    APP_NAME=\"\\\"$${NAME}\\\"\" \
    APP_NAME_NO_SPACES=\"\\\"$${NAME_NO_SPACES}\\\"\"

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT


win32 {
    TARGET = $$NAME_NO_SPACES
    QMAKE_TARGET_PRODUCT = $$NAME
    QMAKE_TARGET_DESCRIPTION = $$NAME
    QMAKE_TARGET_COMPANY = "Inviska Software"
    QMAKE_TARGET_COPYRIGHT = "Released under the GPLv2 licence"
    RC_LANG = 0x0809
    RC_CODEPAGE = 1252
    RC_ICONS = ./Resources/Icon.ico
    QMAKE_LFLAGS *= -static-libgcc -static-libstdc++ -static -lpthread
    QT += winextras
}


macx {
    TARGET = $$NAME_NO_SPACES
    ICON = ./Resources/Icon.icns
    QMAKE_INFO_PLIST = ./Resources/Info.plist
    QMAKE_CFLAGS_RELEASE *= -fvisibility=hidden
    QMAKE_CXXFLAGS_RELEASE *= -fvisibility=hidden -fvisibility-inlines-hidden
}


unix:!macx {
    TARGET = invren
    LIBS += -lz

    QMAKE_CPPFLAGS *= -Wdate-time -D_FORTIFY_SOURCE=2
    QMAKE_CFLAGS   *= -fPIE -g -O2 -fdebug-prefix-map=/home/user=. -fstack-protector-strong -Wformat -Werror=format-security
    QMAKE_CXXFLAGS *= -fPIE -g -O2 -fdebug-prefix-map=/home/user=. -fstack-protector-strong -Wformat -Werror=format-security
    QMAKE_LFLAGS   *= -pie -Wl,-Bsymbolic-functions -Wl,-z,relro,-z,now
}


# For building release from command line - qmake CONFIG+=BuildRelease
contains(CONFIG, BuildRelease) {
    message("Building the RELEASE Version")
    CONFIG -= debug_and_release
    CONFIG -= debug
    CONFIG += release

    OUTPUTDIR = release
    DESTDIR = $$OUTPUTDIR
    OBJECTS_DIR = $$OUTPUTDIR
    MOC_DIR = $$OUTPUTDIR
    RCC_DIR = $$OUTPUTDIR
    UI_DIR = $$OUTPUTDIR
}


# For building debug from command line - qmake CONFIG+=BuildDebug
contains(CONFIG, BuildDebug) {
    message("Building the DEBUG Version")
    CONFIG -= debug_and_release
    CONFIG -= release
    CONFIG += debug

    OUTPUTDIR = debug
    DESTDIR = $$OUTPUTDIR
    OBJECTS_DIR = $$OUTPUTDIR
    MOC_DIR = $$OUTPUTDIR
    RCC_DIR = $$OUTPUTDIR
    UI_DIR = $$OUTPUTDIR
}
