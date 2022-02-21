################################################################################
# Build
################################################################################
QT += core gui uitools widgets
CONFIG += console warn_on
QMAKE_CFLAGS +=  -Wno-array-bounds

DMMSNOOP_APP_SUFFIX = bin
DMMSNOOP_DATA_SUFFIX = share
!isEmpty(DEBUG) {
    CONFIG += debug
    DEFINES += DMMSNOOP_DEBUG
}

isEmpty(BUILDDIR) {
    BUILDDIR = ../build
}
isEmpty(MAKEDIR) {
    MAKEDIR = ../make
}


macx {
    CONFIG += x86_64
    DEFINES += DMMSNOOP_PLATFORM_MACX
} else:unix {
    DEFINES += DMMSNOOP_PLATFORM_UNIX
} else:win32 {
    DEFINES += DMMSNOOP_PLATFORM_WIN32
} else {
    warning(Your platform has not been detected successfully.  Expect errors.)
}

isEmpty(PREFIX) {
    win32 {
        PREFIX = C:/Program Files/dmmsnoop
    } else {
        PREFIX = /usr
    }
}
isEmpty(DATAROOTDIR) {
    DATAROOTDIR = $${PREFIX}/$${DMMSNOOP_DATA_SUFFIX}
}
isEmpty(EXECPREFIX) {
    EXECPREFIX = $${PREFIX}
}
isEmpty(BINDIR) {
    DMMSNOOP_APP_INSTALL_PATH = $${EXECPREFIX}/$${DMMSNOOP_APP_SUFFIX}
} else {
    DMMSNOOP_APP_INSTALL_PATH = $${BINDIR}
}
isEmpty(DATADIR) {
    DMMSNOOP_DATA_INSTALL_PATH = $${DATAROOTDIR}
} else {
    DMMSNOOP_DATA_INSTALL_PATH = $${DATADIR}
}

DESTDIR = $${BUILDDIR}/$${DMMSNOOP_APP_SUFFIX}
HEADERS +=          \
    BuildNo.hpp     \
    AppVersion.hpp  \
    _main.hpp       \
    App.hpp         \
    Ctrl.hpp        \
    Midi.hpp        \
    MtcQf.hpp       \
    DrvIf.hpp       \
    QVwClose.hpp    \
    DelgMsgTbl.hpp  \
    QVwMain.hpp     \
    QVwAbout.hpp    \
    QVwConfig.hpp   \
    QVwMsg.hpp      \
    QVwErr.hpp      \
    QVwDlg.hpp      \
    QVwDesgn.hpp    \
    QVwBase.hpp     \
    util/Error.hpp  \
    util/TrMsg.hpp  \
    util/TrMsgApp.hpp  \
    util/TrMsgMiMeta.hpp  \
    util/TrMsgMiStat.hpp  \
    util/TrMsgMiSys.hpp  \
    util/DmmStr.hpp
LIBS += -lrtmidi
MOC_DIR = $${MAKEDIR}
OBJECTS_DIR = $${MAKEDIR}

RCC_DIR = $${MAKEDIR}
RESOURCES += resources.qrc
SOURCES +=          \
    _main.cpp       \
    App.cpp         \
    Ctrl.cpp        \
    Midi.cpp        \
    MtcQf.cpp       \
    DrvIf.cpp       \
    QVwClose.cpp    \
    DelgMsgTbl.cpp  \
    QVwMain.cpp     \
    QVwAbout.cpp    \
    QVwConfig.cpp   \
    QVwMsg.cpp      \
    QVwErr.cpp      \
    QVwDlg.cpp      \
    QVwDesgn.cpp    \
    QVwBase.cpp     \
    util/Error.cpp  \
    util/TrMsg.cpp  \
    util/DmmStr.cpp
TARGET = dmmsnoop
TEMPLATE = app
QMAKE_POST_LINK += ./BuildInc

################################################################################
# Install
################################################################################

icon.files = template/dmmsnoop.svg

unix:!macx {
    icon.path = $${DMMSNOOP_DATA_INSTALL_PATH}/icons/gnome/scalable/apps

    desktop.CONFIG += no_check_exist
    desktop.extra = ../install/build-desktop-file \
        '$${DMMSNOOP_APP_INSTALL_PATH}' '$${DMMSNOOP_DATA_INSTALL_PATH}'
    desktop.files = ../resources/dmmsnoop.desktop
    desktop.path = $${DMMSNOOP_DATA_INSTALL_PATH}/applications
    INSTALLS += desktop

} else {
    icon.path = $${DMMSNOOP_DATA_INSTALL_PATH}
}
INSTALLS += icon

target.path = $${DMMSNOOP_APP_INSTALL_PATH}
INSTALLS += target
