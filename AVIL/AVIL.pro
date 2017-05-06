TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    avil.cpp \
    sysArduinoUNO.cpp \
    sysPC.cpp \
    avilData.cpp \
    avilExpr.cpp \
    avilTok.cpp

include(deployment.pri)

HEADERS += \
    avil.h \
    compopt.h \
    sys.h \
    sysArduinoUNO.h \
    sysPC.h \
    avilData.h \
    avilExpr.h \
    avilTok.h \
    avilKeywords.h


unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += ncurses
