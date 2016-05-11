#include <iostream>

#include "Build.hh"
#include "Gear.hh"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    Gear g;

    g.pieces[Gear::Head].fix(Rating::Hit);
    g.pieces[Gear::MajorLeft].fix(Rating::Pen);
    g.pieces[Gear::MajorRight].fix(Rating::Pen);
    g.pieces[Gear::MinorLeft].fix(Rating::Hit);

    g.gearStats().dumpDpsGlyphs();

    for (auto const& s : g.enumerateGearStats({Rating::Crit, Rating::CritPower}, false))
        std::cout << s.critRating << ";" << s.critPowerRating << std::endl;

    return 0;
    Build b;
    b.loadPistolShotgunHairTrigger();
    b.dump();
}
