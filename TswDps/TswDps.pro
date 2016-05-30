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
    CombatLog.cc \
    Rotation.cc \
    Skills.cc \
    SkillTable.cc \
    Optimizer.cc \
    Passives.cc \
    jsonxx.cc

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
    Effect.hh \
    Effects.hh \
    Builds.hh \
    SkillTable.hh \
    Optimizer.hh \
    jsonxx.hh \
    results/best/Fist-Blade.json \
    results/best/Hammer-Blade.json \
    results/best/Hammer-Fist.json \
    results/best/Blood-Blade.json \
    results/best/Blood-Fist.json \
    results/best/Blood-Hammer.json \
    results/best/Chaos-Blade.json \
    results/best/Chaos-Fist.json \
    results/best/Chaos-Hammer.json \
    results/best/Chaos-Blood.json \
    results/best/Elemental-Blade.json \
    results/best/Elemental-Fist.json \
    results/best/Elemental-Hammer.json \
    results/best/Elemental-Blood.json \
    results/best/Elemental-Chaos.json \
    results/best/Rifle-Blade.json \
    results/best/Rifle-Fist.json \
    results/best/Rifle-Hammer.json \
    results/best/Rifle-Blood.json \
    results/best/Rifle-Chaos.json \
    results/best/Rifle-Elemental.json \
    results/best/Pistol-Blade.json \
    results/best/Pistol-Fist.json \
    results/best/Pistol-Hammer.json \
    results/best/Pistol-Blood.json \
    results/best/Pistol-Chaos.json \
    results/best/Pistol-Elemental.json \
    results/best/Pistol-Rifle.json \
    results/best/Shotgun-Blade.json \
    results/best/Shotgun-Fist.json \
    results/best/Shotgun-Hammer.json \
    results/best/Shotgun-Blood.json \
    results/best/Shotgun-Chaos.json \
    results/best/Shotgun-Elemental.json \
    results/best/Shotgun-Rifle.json \
    results/best/Shotgun-Pistol.json \
    results/summary.csv

linux {
    QMAKE_CXXFLAGS += -Wno-unused-parameter
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
}
msvc {
    QMAKE_CXXFLAGS += /wd4100
    QMAKE_CXXFLAGS_WARN_ON -= -w34100

    debug {
        exists("C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10240.0\ucrt\x64\ucrtd.lib") {
            INCLUDEPATH += "C:\Program Files (x86)\Windows Kits\10\Include\10.0.10240.0\ucrt"
            LIBS += "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10240.0\ucrt\x64\ucrtd.lib"
        }
    }
    release {
        exists("C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10240.0\ucrt\x64\ucrt.lib") {
            INCLUDEPATH += "C:\Program Files (x86)\Windows Kits\10\Include\10.0.10240.0\ucrt"
            LIBS += "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10240.0\ucrt\x64\ucrt.lib"
        }
    }
}
