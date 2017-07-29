QT += widgets

HEADERS       = JIFS.h \
                renderthread.h \
                jifswidget.h
SOURCES       = main.cpp \
                JIFS.cpp \
                renderthread.cpp \
                jifswidget.cpp

LIBS += -lann -lm

CONFIG += c++11 qt release
