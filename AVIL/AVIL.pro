TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    expressions.cpp \
    tokenizer.cpp \
    avil.cpp \
    arduinoSys.cpp \
    pcSys.cpp \
    data.cpp

include(deployment.pri)

HEADERS += \
    expressions.h \
    keywords.h \
    tokenizer.h \
    avil.h \
    compopt.h \
    sys.h \
    arduinoSys.h \
    pcSys.h \
    data.h

