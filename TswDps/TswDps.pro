QT += core
QT -= gui

CONFIG += c++11

TARGET = TswDps
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cc \
    Build.cc

HEADERS += \
    Build.hh \
    Skill.hh \
    common.hh \
    Skills.hh \
    Weapon.hh \
    Augment.hh \
    Augments.hh \
    Skillset.hh
