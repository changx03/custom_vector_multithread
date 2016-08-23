TEMPLATE = app
CONFIG += console gnu++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

QMAKE_CXXFLAGS += -std=gnu++11 -pthread
LIBS += -pthread
