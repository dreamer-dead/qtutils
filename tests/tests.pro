include (../lib/lib.pri)

QT += testlib
QT -= gui

TARGET = tests
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11
CONFIG += c++14

TEMPLATE = app

SOURCES += tests.cpp \
    connectortests.cpp
