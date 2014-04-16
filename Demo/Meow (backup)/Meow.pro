
QT += core gui

DEFINES += QT_QTCREATORIDE
CONFIG(release, debug|release) {
    DEFINES += QT_NDEBUG
} else {
    DEFINES += QT_DEBUG
}

release: DESTDIR = build/release
debug: DESTDIR = build/debug

OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR = $$DESTDIR/moc
RCC_DIR = $$DESTDIR/qrc
UI_DIR = $$DESTDIR/ui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Meow
TEMPLATE = app

FORMS += mainwindow.ui \
    repositorydialog.ui \
    busydialog.qt.ui

SOURCES += main.cpp \
    mainwindow.cpp \
    repositorydialog.cpp \
    busydialog.cpp

HEADERS += mainwindow.h \
    repositorydialog.h \
    busydialog.h
