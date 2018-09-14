QT += core gui charts multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Qt_RG
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    mainwindow.h \
    plot.h \
    audiodata.h

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    plot.cpp \
    audiodata.cpp


INSTALLS += target

FORMS += \
    mainwindow.ui

RESOURCES += \
    mwbackgroundimg.qrc
LIBS += /usr/local/lib/libfftw3.a
