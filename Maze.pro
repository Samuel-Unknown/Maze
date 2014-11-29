#-------------------------------------------------
#
# Project created by QtCreator 2014-11-26T18:32:40
#
# Программа генерирует лабиринты и позволяет искать кротчайшие пути между двумя точками
#
# Autor: Samuel-Unknown
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Maze
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    maze.cpp \
    square.cpp

HEADERS  += mainwindow.h \
    maze.h \
    square.h

FORMS    += mainwindow.ui
