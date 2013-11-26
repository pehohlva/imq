#-------------------------------------------------
#
# Project created by QtCreator 2013-11-25T09:33:48
#  qt5 build
#-------------------------------------------------

QT       += core xml network
cache()
QT       -= gui

TARGET = imq
CONFIG   += console
CONFIG   -= app_bundle

TARGET = imq
BINDIR = /usr/bin
target.path = $$BINDIR
INSTALLS += target



TEMPLATE = app
HEADERS += OsFile.h
SOURCES += main.cpp OsFile.cpp
