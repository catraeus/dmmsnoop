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
    BUILDDIR = ../../build
}
isEmpty(MAKEDIR) {
    MAKEDIR = ../../make/util
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

CONFIG += console warn_on
DEFINES += DMMSNOOP_MAJOR_VERSION=$${MAJOR_VERSION} \
    DMMSNOOP_MINOR_VERSION=$${MINOR_VERSION} \
    DMMSNOOP_REVISION=$${REVISION}
DESTDIR = $${BUILDDIR}/$${DMMSNOOP_APP_SUFFIX}
HEADERS += util.hpp
LIBS += -lrtmidi
MOC_DIR = $${MAKEDIR}
OBJECTS_DIR = $${MAKEDIR}
QT += core gui uitools widgets
RCC_DIR = $${MAKEDIR}
RESOURCES += ../resources.qrc
SOURCES += util.cpp
TARGET = dmmsnoop
TEMPLATE = app
VERSION = $${DMMSNOOP_VERSION}

################################################################################
# Install
################################################################################


