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
    results/summary-best.csv \
    results/dummy/Fist-Blade.json \
    results/dummy/Hammer-Blade.json \
    results/dummy/Hammer-Fist.json \
    results/dummy/Blood-Blade.json \
    results/dummy/Blood-Fist.json \
    results/dummy/Blood-Hammer.json \
    results/dummy/Chaos-Blade.json \
    results/dummy/Chaos-Fist.json \
    results/dummy/Chaos-Hammer.json \
    results/dummy/Chaos-Blood.json \
    results/dummy/Elemental-Blade.json \
    results/dummy/Elemental-Fist.json \
    results/dummy/Elemental-Hammer.json \
    results/dummy/Elemental-Blood.json \
    results/dummy/Elemental-Chaos.json \
    results/dummy/Rifle-Blade.json \
    results/dummy/Rifle-Fist.json \
    results/dummy/Rifle-Hammer.json \
    results/dummy/Rifle-Blood.json \
    results/dummy/Rifle-Chaos.json \
    results/dummy/Rifle-Elemental.json \
    results/dummy/Pistol-Blade.json \
    results/dummy/Pistol-Fist.json \
    results/dummy/Pistol-Hammer.json \
    results/dummy/Pistol-Blood.json \
    results/dummy/Pistol-Chaos.json \
    results/dummy/Pistol-Elemental.json \
    results/dummy/Pistol-Rifle.json \
    results/dummy/Shotgun-Blade.json \
    results/dummy/Shotgun-Fist.json \
    results/dummy/Shotgun-Hammer.json \
    results/dummy/Shotgun-Blood.json \
    results/dummy/Shotgun-Chaos.json \
    results/dummy/Shotgun-Elemental.json \
    results/dummy/Shotgun-Rifle.json \
    results/dummy/Shotgun-Pistol.json \
    results/summary-dummy.csv \
    results/burst/Fist-Blade.json \
    results/burst/Hammer-Blade.json \
    results/burst/Hammer-Fist.json \
    results/burst/Blood-Blade.json \
    results/burst/Blood-Fist.json \
    results/burst/Blood-Hammer.json \
    results/burst/Chaos-Blade.json \
    results/burst/Chaos-Fist.json \
    results/burst/Chaos-Hammer.json \
    results/burst/Chaos-Blood.json \
    results/burst/Elemental-Blade.json \
    results/burst/Elemental-Fist.json \
    results/burst/Elemental-Hammer.json \
    results/burst/Elemental-Blood.json \
    results/burst/Elemental-Chaos.json \
    results/burst/Rifle-Blade.json \
    results/burst/Rifle-Fist.json \
    results/burst/Rifle-Hammer.json \
    results/burst/Rifle-Blood.json \
    results/burst/Rifle-Chaos.json \
    results/burst/Rifle-Elemental.json \
    results/burst/Pistol-Blade.json \
    results/burst/Pistol-Fist.json \
    results/burst/Pistol-Hammer.json \
    results/burst/Pistol-Blood.json \
    results/burst/Pistol-Chaos.json \
    results/burst/Pistol-Elemental.json \
    results/burst/Pistol-Rifle.json \
    results/burst/Shotgun-Blade.json \
    results/burst/Shotgun-Fist.json \
    results/burst/Shotgun-Hammer.json \
    results/burst/Shotgun-Blood.json \
    results/burst/Shotgun-Chaos.json \
    results/burst/Shotgun-Elemental.json \
    results/burst/Shotgun-Rifle.json \
    results/burst/Shotgun-Pistol.json \
    results/summary-burst.csv

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
