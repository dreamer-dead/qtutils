include (../lib/lib.pri)

QT += testlib
QT -= gui

TARGET = tests
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

SOURCES += tests.cpp
