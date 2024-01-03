QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dbmanager.cpp \
    entrydelegate.cpp \
    entrymodel.cpp \
    exporter.cpp \
    main.cpp \
    mainwindow.cpp \
    messagebox.cpp \
    typescheduler.cpp

HEADERS += \
    dbmanager.h \
    dbmanagertraverser.h \
    entrydelegate.h \
    entrymodel.h \
    exporter.h \
    globals.h \
    mainwindow.h \
    messagebox.h \
    typeenum.h \
    typescheduler.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    myspending_de_CH.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc
