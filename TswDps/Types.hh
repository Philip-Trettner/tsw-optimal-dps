#pragma once

enum class SubType {
    None,
    Burst,
    Strike,
    Focus,
    Frenzy
};

enum class DmgType {
    None,
    Melee,
    Magic,
    Ranged,

    Count
};

enum class SkillType {
    None,
    Builder,
    Consumer,
    Elite,
    PassiveFullHit,
    Proc
};

enum class PassiveType {
    None,

    Skill,
    Elite
};
