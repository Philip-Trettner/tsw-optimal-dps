#include <iostream>

#include "Build.hh"


int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    Build b;
    b.loadPistolShotgunHairTrigger();
    b.dump();
}
