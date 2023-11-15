QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp

## LINUX ##
unix {
    QT += widgets
}

## WINDOWS ###
win32 {
    QT += openglwidgets
    LIBS += -lopengl32
}


CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    energychart.cpp \
    globalVariables.cpp \
    main.cpp \
    mainwindow.cpp \
    openglsimulation.cpp \
    particlemonitor.cpp \
    particleproperties.cpp \
    sph_functions.cpp \
    ui_functions.cpp

HEADERS += \
    energychart.h \
    globalVariables.h \
    mainwindow.h \
    openglsimulation.h \
    particlemonitor.h \
    particleproperties.h \
    sph_functions.h \
    ui_functions.h

FORMS += \
    mainwindow.ui

# Dirección de las librerias
 INCLUDEPATH += ./include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc

