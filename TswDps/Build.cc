#include "Build.hh"

#include "SkillTable.hh"

#include "Simulation.hh"
#include "CombatLog.hh"
#include "Skills.hh"
#include "Passives.hh"
#include "Augments.hh"

#include <fstream>
#include <iostream>

static string shortStatDump(Gear::Piece const& p)
{
    return shortStatDump(p.stats, false);
}

void Build::shortDump() const
{
    std::cout << "Actives:  ";
    auto first = true;
    for (auto i = 0; i < SKILL_CNT; ++i)
    {
        auto const& s = skills.skills[i];
        if (!s.name.empty())
        {
            if (first)
                first = false;
            else
                std::cout << ", ";
            std::cout << s.name;
            if (!skills.augments[i].name.empty())
                std::cout << " [" << skills.augments[i].name << "]";
        }
    }
    std::cout << std::endl;

    std::cout << "Passives: ";
    first = true;
    for (auto const& s : skills.passives)
        if (!s.name.empty())
        {
            if (first)
                first = false;
            else
                std::cout << ", ";
            std::cout << s.name;
        }
    std::cout << std::endl;

    std::cout << "Gear:     ";
    auto s = gear.gearStats();
    std::cout << s.attackRating << " AR, ";
    std::cout << s.hitRating << " Hit, ";
    std::cout << s.penRating << " Pen, ";
    std::cout << s.critRating << " CR, ";
    std::cout << s.critPowerRating << " CP, ";
    std::cout << "Neck: ";
    if (gear.pieces[Gear::MajorMid].signet.passive.effect == EffectSlot::MothersWrathStacks)
        std::cout << "Woodcutters";
    else if (gear.pieces[Gear::MajorMid].signet.passive.effect == EffectSlot::EgonPendant)
        std::cout << "Amulet of Yuggoth";
    else
        std::cout << "QL11 + Violence";
    std::cout << ", Finger: ";
    if (gear.pieces[Gear::MajorLeft].signet.passive.effect == EffectSlot::ConeyIslandBand)
        std::cout << "Coney";
    else
        std::cout << "QL11 + Violence";
    if (potionStats != Stats())
        std::cout << ", Potion: " << shortStatDump(potionStats, false);
    if (gear.stimulant != EffectSlot::Count)
        std::cout << ", Stim: " << to_string(gear.stimulant);
    if (gear.kickback.passivetype == PassiveType::Kickback)
        std::cout << ", KB: " << gear.kickback.name;
    std::cout << std::endl;

    std::cout << "Weapons:  ";
    std::cout << shortStatDump(gear.pieces[Gear::WeaponLeft]) << " for " << to_string(gear.leftWeapon) << ", ";
    std::cout << shortStatDump(gear.pieces[Gear::WeaponRight]) << " for " << to_string(gear.rightWeapon);
    std::cout << std::endl;

    std::cout << "Signets:  ";
    std::cout << gear.pieces[Gear::Head].signet.name() << " on Head, ";
    std::cout << gear.pieces[Gear::WeaponLeft].signet.name() << " on " << to_string(gear.leftWeapon) << ", ";
    std::cout << gear.pieces[Gear::WeaponRight].signet.name() << " on " << to_string(gear.rightWeapon);
    std::cout << ", Minors: [";
    std::cout << gear.pieces[Gear::MinorLeft].signet.name() << ", ";
    std::cout << gear.pieces[Gear::MinorMid].signet.name() << ", ";
    std::cout << gear.pieces[Gear::MinorRight].signet.name() << "]";
    std::cout << std::endl;

    auto defRot = std::dynamic_pointer_cast<DefaultRotation>(rotation);
    if (defRot)
    {
        std::cout << "Rotation: ";
        std::cout << "use resources on " << defRot->minResourcesForLeftConsumer << " for " << to_string(gear.leftWeapon)
                  << " and " << defRot->minResourcesForRightConsumer << " for " << to_string(gear.rightWeapon);
        if (defRot->tryToConsumeOnBuffed)
            std::cout << " (But prefer consume on buffs)";
        std::cout << ", buffs are: [ ";
        if (defRot->considerBuffEF)
            std::cout << "EF ";
        if (defRot->considerBuffFF)
            std::cout << "FF ";
        if (defRot->considerBuffWC)
            std::cout << "WC ";
        std::cout << "]";
        if (defRot->consumeIfNotBloodOffering && (gear.leftWeapon == Weapon::Blood || gear.rightWeapon == Weapon::Blood))
            std::cout << ", force Blood Offering uptime";
        if (defRot->buildOnlyBeforeMajorBuffs)
            std::cout << ", only build shortly before DABS";
        std::cout << std::endl;
    }

    std::cout << VDM() << std::endl;
}

string Build::VDM() const
{
    /* VDM%-%abil1%-%abil2%-%abil3%-%abil4%-%abil5%-%abil6%-%abil7%-%auxabil%-
        %pas1%-%pas2%-%pas3%-%pas4%-%pas5%-%pas6%-%pas7%-%auxpas%-
        %aug1%-%aug2%-%aug3%-%aug4%-%aug5%-%aug6%-%aug7 */

    std::string vdm = "VDM%";
    for (auto i = 0u; i < 8; ++i)
    {
        std::string id = "undefined";
        if (!skills.skills[i].name.empty())
            id = SkillTable::vdmID(skills.skills[i].name);
        vdm += "-%" + id + "%";
    }
    for (auto i = 0u; i < 8; ++i)
    {
        std::string id = "undefined";
        if (i < skills.passives.size() && !skills.passives[i].name.empty())
            id = SkillTable::vdmID(skills.passives[i].name);
        vdm += "-%" + id + "%";
    }
    for (auto i = 0u; i < 7; ++i)
    {
        std::string id = "undefined";
        if (!skills.augments[i].name.empty())
            id = SkillTable::vdmID(skills.augments[i].name + " Augment Resonator");
        vdm += "-%" + id + "%";
    }

    return vdm;
}

namespace {

struct BuildRotationLog : CombatLog
{
    std::vector<int> skills;

    void logSkill(Simulation* sim, int timeIn60th, int skillIdx) override
    {
        skills.push_back(skillIdx);
    }
};
}

jsonxx::Object Build::toJson() const
{
    jsonxx::Array ss;
    jsonxx::Array aa;
    for (auto i = 0; i < SKILL_CNT; ++i)
    {
        ss << skills.skills[i].name;
        aa << skills.augments[i].name;
    }
    jsonxx::Array pp;
    for (auto const& p : skills.passives)
        pp << p.name;
    jsonxx::Array gg;
    for (auto const& p : gear.pieces)
        gg << p.toJson();
    jsonxx::Object b;
    b << "Skills" << ss;
    b << "Augments" << aa;
    b << "Passives" << pp;
    b << "Left Weapon" << to_string(gear.leftWeapon);
    b << "Right Weapon" << to_string(gear.rightWeapon);
    b << "Gear" << gg;
    auto rot = std::dynamic_pointer_cast<DefaultRotation>(rotation);
    assert(rot && "not supported");
    b << "Rotation" << rot->toJson();
    b << "Potion" << potionStats.toJson();
    if (gear.stimulant != EffectSlot::Count)
        b << "Stim" << to_string(gear.stimulant);
    if (!gear.kickback.name.empty())
        b << "Kickback" << gear.kickback.name;

    // MISC
    {
        jsonxx::Object m;

        // stim
        if (gear.stimulant != EffectSlot::Count)
            for (auto const& e : Effects::Stimulants::all())
                if (e.slot == gear.stimulant)
                    m << "Stim" << e.bonusStats.toJson();

        // kickback
        if (!gear.kickback.name.empty())
            for (auto const& e : Effects::Kickbacks::all())
                if (e.slot == gear.kickback.effect)
                    m << "Kickback" << e.bonusStats.toJson();

        // vdm
        m << "VDM" << VDM();

        // rotation
        {
            Simulation s;
            s.loadBuild(*this);
            BuildRotationLog l;
            s.log = &l;
            s.simulate(20 * 60); // 20s
            // cull builders
            auto ss = l.skills;
            while (ss.size() > 10 && skills.skills[ss.back()].skilltype == SkillType::Builder)
                ss.erase(end(ss) - 1);
            // add rotation
            jsonxx::Array a;
            for (auto const& s : ss)
                a << skills.skills[s].name;
            m << "Initial Rotation" << a;
        }

        // .. fin
        b << "Misc" << m;
    }

    return b;
}

void Build::fromJson(const jsonxx::Object& o)
{
    using namespace jsonxx;
    auto ss = o.get<Array>("Skills");
    auto aa = o.get<Array>("Augments");
    auto pp = o.get<Array>("Passives");
    auto gg = o.get<Array>("Gear");

    if (o.has<String>("Stim"))
    {
        auto ss = o.get<String>("Stim");
        for (auto i = 0; i < (int)EffectSlot::Count; ++i)
            if (to_string((EffectSlot)i) == ss)
                gear.stimulant = (EffectSlot)i;
    }

    if (o.has<String>("Kickback"))
    {
        auto kb = o.get<String>("Kickback");
        for (auto const& p : Passives::Kickback::all())
            if (kb == p.name)
                gear.kickback = p;
    }

    if (o.has<Object>("Potion"))
    {
        potionStats = Stats();
        potionStats.fromJson(o.get<Object>("Potion"));
    }

    if (o.has<Number>("Left Weapon"))
        gear.leftWeapon = (Weapon)(int)o.get<Number>("Left Weapon", (int)gear.leftWeapon);
    else
        gear.leftWeapon = parseWeapon(o.get<String>("Left Weapon", to_string(gear.leftWeapon)));

    if (o.has<Number>("Right Weapon"))
        gear.rightWeapon = (Weapon)(int)o.get<Number>("Right Weapon", (int)gear.rightWeapon);
    else
        gear.rightWeapon = parseWeapon(o.get<String>("Right Weapon", to_string(gear.rightWeapon)));


    auto allSkills = Skills::all();
    auto allAugs = Augments::all();
    auto allPassives = Passives::all();

    skills.passives.resize(pp.size());

    for (auto i = 0u; i < ss.size(); ++i)
    {
        auto s = ss.get<String>(i);
        if (s.empty())
            skills.skills[i] = Skills::empty();
        else
        {
            // remapping
            if (s == "Cannibalize")
                s = "Cannibalise";
            if (s == "Big 45")
                s = "Big Forty Five";

            bool found = false;
            for (auto const& sr : allSkills)
                if (sr.name == s)
                {
                    skills.skills[i] = sr;
                    found = true;
                    break;
                }
            if (!found)
                std::cerr << "Unknown skill " << s << std::endl;
        }
    }

    for (auto i = 0u; i < aa.size(); ++i)
    {
        auto const& s = aa.get<String>(i);
        if (s.empty())
            skills.augments[i] = Augments::empty();
        else
        {
            bool found = false;
            for (auto const& ar : allAugs)
                if (ar.name == s)
                {
                    skills.augments[i] = ar;
                    found = true;
                    break;
                }
            if (!found)
                std::cerr << "Unknown augment " << s << std::endl;
        }
    }

    for (auto i = 0u; i < pp.size(); ++i)
    {
        auto const& s = pp.get<String>(i);
        if (s.empty())
            skills.passives[i] = Passives::empty();
        else
        {
            bool found = false;
            for (auto const& pr : allPassives)
                if (pr.name == s)
                {
                    skills.passives[i] = pr;
                    found = true;
                    break;
                }
            if (!found)
                std::cerr << "Unknown passive " << s << std::endl;
        }
    }

    for (auto i = 0u; i < gg.size(); ++i)
    {
        auto const& go = gg.get<Object>(i);
        gear.pieces[i].fromJson(go);
    }

    auto defRot = std::dynamic_pointer_cast<DefaultRotation>(rotation);
    if (!defRot)
        defRot = DefaultRotation::create();
    defRot->fromJson(o.get<Object>("Rotation"));
    rotation = defRot;
}

void Build::fromFile(const string& filename)
{
    std::ifstream file(filename);
    if (!file.good())
    {
        std::cerr << "Unable to open " << filename;
        return;
    }
    jsonxx::Object o;
    o.parse(file);
    fromJson(o);
}
