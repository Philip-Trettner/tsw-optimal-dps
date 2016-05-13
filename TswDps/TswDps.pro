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
    Gear.cc \
    CombatLog.cc

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
    Gear.hh \
    Signets.hh \
    CombatLog.hh \
    Effect.hh

linux {
    QMAKE_CXXFLAGS += -Wno-unused-parameter
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
}
msvc {
    QMAKE_CXXFLAGS += /wd4100
    QMAKE_CXXFLAGS_WARN_ON -= -w34100
}
