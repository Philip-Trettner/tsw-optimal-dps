QT += core
QT -= gui

CONFIG += c++11

TARGET = TswDps
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cc \
    Build.cc \
    Stats.cc \
    Simulation.cc \
    utils.cc \
    Gear.cc

HEADERS += \
    Build.hh \
    Skill.hh \
    common.hh \
    Skills.hh \
    Weapon.hh \
    Augment.hh \
    Augments.hh \
    Skillset.hh \
    Passive.hh \
    Types.hh \
    Passives.hh \
    Stats.hh \
    Potions.hh \
    EnemyInfo.hh \
    Simulation.hh \
    Rotation.hh \
    Signet.hh \
    utils.hh \
    Gear.hh
