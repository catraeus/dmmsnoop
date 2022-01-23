################################################################################
# Build
################################################################################

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

isEmpty(MAJOR_VERSION) {
    MAJOR_VERSION = 9
}
isEmpty(MINOR_VERSION) {
    MINOR_VERSION = 9
}
isEmpty(REVISION) {
    REVISION = 9
}
DMMSNOOP_VERSION = $${MAJOR_VERSION}.$${MINOR_VERSION}.$${REVISION}

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
        PREFIX = /usr/local
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

CONFIG += console warn_on
DEFINES += DMMSNOOP_MAJOR_VERSION=$${MAJOR_VERSION} \
    DMMSNOOP_MINOR_VERSION=$${MINOR_VERSION} \
    DMMSNOOP_REVISION=$${REVISION}
DESTDIR = $${BUILDDIR}/$${DMMSNOOP_APP_SUFFIX}
HEADERS += QVwAbout.hpp \
    TheApp.hpp \
    closeeventfilter.h \
    QVwConfig.hpp \
    controller.h \
    designerview.h \
    dialogview.h \
    engine.h \
    error.h \
    QVwError.hpp \
    QVwMain.hpp \
    messagetabledelegate.h \
    messageview.h \
    util.h \
    view.h
LIBS += -lrtmidi
MOC_DIR = $${MAKEDIR}
OBJECTS_DIR = $${MAKEDIR}
QT += core gui uitools widgets
RCC_DIR = $${MAKEDIR}
RESOURCES += resources.qrc
SOURCES += QVwAbout.cpp \
    TheApp.cpp \
    closeeventfilter.cpp \
    QVwConfig.cpp \
    controller.cpp \
    designerview.cpp \
    dialogview.cpp \
    engine.cpp \
    error.cpp \
    QVwError.cpp \
    _main.cpp \
    QVwMain.cpp \
    messagetabledelegate.cpp \
    messageview.cpp \
    util.cpp \
    view.cpp
TARGET = dmmsnoop
TEMPLATE = app
VERSION = $${DMMSNOOP_VERSION}

################################################################################
# Install
################################################################################

icon.files = template/dmmsnoop.svg

unix:!macx {
    icon.path = $${DMMSNOOP_DATA_INSTALL_PATH}/icons/

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
