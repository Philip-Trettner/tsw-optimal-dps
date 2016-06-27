See https://forums-tl.thesecretworld.com/misc.php?do=bbcode

Table: [table]
[tr]
	[th]A[/th]
[/tr]
[tr]
	[td]A[/td]
[/tr]
[/table]
[color=#RRGGBB][/color]

================================================================================
Title:
ArtificialMind's Ultimate Theorycrafted DPS Guide [1.15.2 with Elite Revamp]

================================================================================
TODO:

Sections:
 * The DPS Formula
 * A Combat Simulator + How the builds got made + Explain how to read builds/rotation
 * FAQ + Changelog + Known Issues + pieces of wisdom
 * TODO list

================================================================================
Content:

[size=+2][color=#03A9F4]ArtificialMind's Ultimate Theorycrafted DPS Guide[/color][/size]
[i][color=gray]Last Update: June 27th, 2016 (1.15.2 with Elite Revamp)[/color][/i]


[size=+2][color=#03A9F4]TL;DR: all builds can be found at [url]http://www.markdijkema.nl/projects/tswbuildstatistics[/url][/color][/size]
[i][color=gray](Builds by ArtificialMind, Website by DijkeMark)[/color][/i]


Now that that's out of the way, welcome to my end-game theorycrafted DPS guide!

I've spent a considerable amount of time writing a combat simulator for TSW that accurately captures all mechanics and effects required for evaluating single-target DPS.
For that, I've reverse engineered most of the DPS formula.
The next step was to automatically switch any aspect of a build, re-evaluate its DPS and - if higher - update the original build.
With this mechanic, my program is able to automatically find really high DPS builds for every weapon combination under various situations.
The results can be found at the website linked above (thanks again, Mark, for setting up the site ;) ).

The rest of this guide is structured as follows:
[list]
[*] How to Read the Builds
[*] The DPS Formula
[*] FAQ
[*] Changelog, Known Issues, Future Work
[/list]


================================================================================
[size=+2][color=#03A9F4]How to Read the Builds[/color][/size]
[i][color=gray](Or: How to use http://www.markdijkema.nl/projects/tswbuildstatistics)[/color][/i]


[size=+1][color=#B2EBF2]Build Types[/color][/size]
The left pane contains a navigation by build type.
A build type (or scenario or setting) is the environment used to evaluate the build.

Currently, the following types are implemented and optimized:

[list]
[*][color=#03A9F4]Raid[/color]: An exposed raid boss with all vulnerabilities applied. Each fight lasts 2.5 minutes with DABS every 60-80 seconds. No SF, no Whip, no Keziah Mason's Ring, no Brace of the Ghoul King.
[*][color=#03A9F4]Raid Melee/Ranged/Magic[/color]: Same as Raid, but your elite has to apply the proper vulnerability. Useful for setting up a raid.
[*][color=#03A9F4]Burst[/color]: Same basic setting as Raid, but with 25s fights.
[*][color=#03A9F4]Dummy[/color]: No expose, no vulnerability applied (except what you might bring on your own), no DABS or other buffs. Each fight lasts 2.5 minutes. Assumes an affliction and a non-expose weaken (for signets and passives).
[*][color=#03A9F4]Percentage[/color]: Same as Raid, but proc passives are forbidden (Live Wire, Thunderstruck, Sudden Return, One in the Chamber, Gnosis, Fortunate Strike).
[*][color=#03A9F4]Budget[/color]: Same as Raid, but with 10.9.5 gear, no Woodcutters, no NM raid drops, and no Flappy drops (purple gadgets).
[/list]
(Note that all settings except Budget assume full QL11 gear)


[size=+1][color=#B2EBF2]Build Type Overview[/color][/size]
After clicking on a build type, the main content area will show an overview over all Weapon Combinations.
In both tabular and list form, the DPS of each combination is shown.
Clicking on the DPS will show you the Build.

Apart from the builds, the overview also contains the Average Glyph Stats for this build type as well as the most used passives.


[size=+1][color=#B2EBF2]A Particular Build[/color][/size]
Clicking on a DPS number in the overview shows you the actual build with a detailed breakdown of (hopefully) all information that you desire.

The analysis of a build actually uses more time than what is allocated for evaluating a build during optimization.
The two lines after the title show you an approximation of how accurate the analysis/optimization was.

A VDM string is provided so you can conveniently import the build.


[size=+1][color=#B2EBF2]Build[/color][/size]
Probably the most important part of a build are the actives, augments, and passives.
Each part of the build (except builder) has an associated dps increase, which means how much dps would you lose if you were to not use that active/aug/passive.

Note that especially augments are sometimes silly.
Like an Invulnerable on Full Momentum without Equilibrium or CiB.
The reason is that even after optimizing for a long time (and simulating 30h+ fighting), RNG still causes a variance of 0.3-0.4%.
Most Augments increase your DPS by less (or around) that value, so by sheer RNG, my algorithm could chose the worse setup due to RNG.


[size=+1][color=#B2EBF2]Gear[/color][/size]
Next important part is the gear setup (signets and glyphs).
The Gear table shows each piece with its signet.
Every signet is associated with a dps increase as well (which is the dps you'd lose if that slot would be empty).
The next table shows the overall gear stats (split by weapon if required).

For Neck (and sometimes Finger), alternative signets are shown.
These are basically Woodcutters, .9 Egon, or QL11 + Violence (depending on what is already equipped).
Their associated dps is what you'd lose if you would equip that talisman over the recommended one.

My optimization doesn't care if the signets on head/weapon are convenient for you or not.
Often, Laceration is placed on a weapon and some weird Fury/Sadism/Aggression on head and the other weapon.
For your convenience, the "Alternative Signet Placement" section shows what would happen if you use a "sane" signet distribution.


[size=+1][color=#B2EBF2]Extras[/color][/size]
The small "Extras" section contains the recommended Potion as well as Stimulant and Kickback.
Note that with the exception of the "Budget" setting, all Stims and Kickbacks are purple Flappy drops.


[size=+1][color=#B2EBF2]Rotation[/color][/size]
The next two parts are about the Rotation.
My Rotations are rule-based (something that might change in the future though).
For your convenience, the first 20-something seconds of each build are shown.
In the longer settings (all except burst), those initial rotation can be a bit silly because they impact DPS by so little.
Initial rotations in the burst setting are typically better optimized.

The rules of the rotation should be described properly (we had some iterations on the wording already but suggestions are welcome ;) ).
One rule that is not explicitly stated is that you should use your cooldowns during DABS and in a buffed state (which - depending on the build - is either Elemental Force or Woodcutters or both).
Bombardment and Power-Line should be used off cooldown.


[size=+1][color=#B2EBF2]Damage Breakdown[/color][/size]
After the rotation, the next two tables show a breakdown of your damage by source and by type.
Those are purely informational but can be used to decide which vulnerability is the most important.


[size=+1][color=#B2EBF2]Glyph Stat Impact[/color][/size]
One of my favorite tables, the glyph stat impact show what happens if you change or switch around stats.
This is super useful to evaluate if your particular glyph setup is "bad" when you plan your gear for multiple builds.
In most cases, it's fine to switch around 200 crit/pen/cp without more than 0.5% dps impact.
Often you can even improve the recommended stats by moving some stats around due to all the RNG involved in analysis as well as optimization.
So take the stats with a grain of salt.

To make this clear again: if you are near (+- 200 stats) the optimal glyph setup, even my 30h+ combat time simulation cannot detect the difference properly.
If you're wondering if you should reglyph just to switch 100 stats around: Don't do it, it's a waste ;)

Another tip: sometimes the non-builder weapon has silly stats, like 200 less hit or something.
The impact is probably so low that the optimization doesn't really detect it. So please think for yourself if you see weird weapon splits ;)


================================================================================
[size=+2][color=#03A9F4]The DPS Formula[/color][/size]

One of the corner stones of the Combat Simulator is a proper formula for DPS.


[size=+1][color=#B2EBF2]Combat Power[/color][/size]

This one have been known for quite some while now.
In some past patch, they made Combat Power scale better once you hit 5k AR.

If you have below 5200 Attack Rating:
[quote][size=+1]combatPower = (375 - 600 / (exp(attackRating / 1400) + 1)) * (1 + weaponPower / 375)[/size][/quote]
otherwise
[quote][size=+1]combatPower = 204.38 + .5471 * weaponPower + (.00008 * weaponPower + .0301) * attackRating[/size][/quote]


[size=+1][color=#B2EBF2]Crit Chance[/color][/size]

Crit Chance is also already known because you can just look in your character sheet.
The approximation I've used is
[quote][size=+1]crit % = 0.6511 - 1.202 / (exp(critRating / 790.3) + 1)[/size][/quote]
If the Egon Pendant is active, the formula becomes
[quote][size=+1]crit % = 0.5514 - 1.003 / (exp(critRating / 790.3) + 1)[/size][/quote]


[size=+1][color=#B2EBF2]Crit Power[/color][/size]

Similar to Crit Chance, you can just look this up:
[quote][size=+1]crit power % = sqrt(5 * critPowerRating + 625) / 100.0[/size][/quote]


[size=+1][color=#B2EBF2]Pen and Block Chance[/color][/size]

Now this is more interesting.
After parsing in the dummy chamber some time and testing in the fight club, it seemed to me that the pen chance depends on "Pen Rating - Block Rating".
Similarly, block chance depends on "Block Rating - Pen Rating".
Both have a diminishing returns formula similar to crit rating.
Additive pen chance (like 15% from Iron Maiden) is added to your pen chance and subtracted from your block chance.

Testing in the fight club allowed me to verify that the point where you get 0% blocked is when your pen rating is about 390 higher than the enemies block.
With that rule we went into Eidolon NM and checked where that threshold is.
Turns out, it's 860 pen, meaning Eidolon has about 470 block there.
If you do this for the test dummies, you'll notice that they have about 100 block.
Knowing this, I could fit the crit rating formula against pen chance and block chance:

[quote][size=+1]pen % = 0.312787 - 0.426939 / (exp((penRating - enemy.blockRating) / 388.004) + 1)[/size][/quote]
[quote][size=+1]block % = 0.28291 - 0.362406 / (exp((enemy.blockRating - penRating) / 312.481) + 1[/size][/quote]

If you want to contribute block ratings for other bosses, find the point where you get 0% blocks (without Woodcutters, without Iron Maiden) as close as you can.
Subtract 390 from your current pen rating and you've found the enemy's block rating.


[size=+1][color=#B2EBF2]Glance Chance[/color][/size]

Doing the same as for pen and block, I've checked the hit rating version enemy defense rating.

This gave me the following formula for glance chance:
[quote][size=+1]glance % = 0.167429 - 0.193239 / (exp((enemy.defenseRating - hitRating) / 150.874) + 1[/size][/quote]

Defense for dummies is 100 and 350 for bosses that have the usual 650 hit cap.
(Note that I've spent less time with this than with the pen rating, so the enemy defense could be shifted by some amount. The relative difference between dummy and normal bosses is the same though.)


[size=+1][color=#B2EBF2]Evade Chance[/color][/size]

Unfortunately, I don't have reversed Evade Chance yet.
Please see the section in Future Work.


[size=+1][color=#B2EBF2]Skill Scaling[/color][/size]

The actual damage of a skill is determined by the current combat power and the skill scaling.
[quote][size=+1]Base Damage = Combat Power * Skill Scaling[/size][/quote]
I've captured all skill scalings that are important for me here: https://docs.google.com/spreadsheets/d/1z9b23xHPNQuqmZ5t51SeIMq2rlI6d8mPyWp9BmGNxjc


================================================================================
[size=+2][color=#03A9F4]FAQ[/color][/size]

[size=+1][color=#B2EBF2]What Stats do you recommend?[/color][/size]
That depends on the build. But for normal proc builds I would go:

[i]10.9.5 Talismans with Woodcutters:[/i]
656 hit (two major)
574 pen (head + minor)
726 crit (weapons + 2x minor)
296 crit power (WC only)

[i]10.9.5 Talismans (no WC):[/i]
656 hit (two major)
679 pen (head + minor + half a minor)
628 crit (weapons + one and a half minor)
296 crit power (WC only)

[i]QL11:[/i]
643 hit (head + minor)
603 pen (major + minor)
689 crit (weapon + major)
497 crit power (WC + minor)

Those values seem to work well with most builds.

[size=+1][color=#B2EBF2]So ... I have to play Ele/X now?[/color][/size]

Not really.

Elemental with Manifestations and Power-Line is really strong and I've seen some amazing dps with it.
However, manis are hard to place and you often lose your target and these builds generate more lag than usual.
So in the end, it's a hard and sometimes annoying way to play but it deals high damage.

Nevertheless, most other builds also deal high dps, so just take whatever you like ;)

[size=+1][color=#B2EBF2]Will I do 9700 DPS in Flappy if I use your blood/ele build?[/color][/size]

No.

That is the sustained damage without downtime and with optimal vulnerability uptime.

Also, as I'll mention in the Future Work section, this DPS does not take enemy protection into account.


================================================================================
[size=+2][color=#03A9F4]Changelog[/color][/size]

[size=+1][color=#B2EBF2]Version 1.0[/color][/size]

[list]
[*] Initial Release
[*] Optimizing Combat Simulator for builds after the Elite Revamp
[*] Build types: 'raid', 'raid-vuln X/Y/Z', 'burst 25s', 'budget', 'percentage', 'dummy'
[*] Build website at http://www.markdijkema.nl/projects/tswbuildstatistics (by DijkeMark)
[/list]



[size=+2][color=#03A9F4]Known Issues[/color][/size]

[list]
[*] The optimizer can miss some micro-optimizations (e.g. still having weapon-specific minor signets even though they are clearly worse) due to RNG. It's typically minor stuff with less than 1% impact on DPS.
[*] Similar to the last issue, sometimes Augments are messed up (e.g. a Curing even though neither CiB or Equilibrium is used), because no Augment in that slot would provide more than 0.5% dps (and that potential could be missed due to RNG).
[*] Power-Line is activated regardless of any ability that may currently be channeling. It's not a huge problem because most builds don't include channels longer than 1s anymore.
[*] No Evades (see Future Work)
[*] Skill effects that are triggered on Impaired/Hindered targets are ignored. If your whole raid uses hinders and impairs, the boss will be immune most of the time anyways, so I chose to ignore them for now.
[*] Double Up (pistol skill passive) and Calamity (chaos skill passive) are not logged (most of the time) by the game and thus don't show up on any DPS meter or ACT parse. I chose to ignore them because - at least for me - non-measurable DPS doesn't really matter.
[*] Sometimes, builds take pretty long to be optimized and may end up in different states of optimization (discrepancy between raid and raid-vuln). The most drastic I've found is Blade/Elemental in Raid vs Raid-Ranged.
[*] Destiny is not implemented yet, so blade builds often use Binding Wounds. Do you tank a favor and take Destiny instead ;)
[/list]



[size=+2][color=#03A9F4]Future Work[/color][/size]

[size=+1][color=#B2EBF2]Evade Formula[/color][/size]
Afflictions and Procs cannot be evaded, making it harder to get a proper sample size for reverse engineering the Evade Formula.
Right now most builds use high hit (500+) anyways, so it's probably not an issue.
For completionist reasons I would still like to have that formula.

[size=+1][color=#B2EBF2]Physical/Magical Protection[/color][/size]
Right now I'm assuming that the displayed skill damage is not reduced by the boss.
It's probably irrelevant for comparing builds, but for completion (and maybe even verification) purposes, knowing the formula and values for protection would be nice.

[size=+1][color=#B2EBF2]Dungeon-specific builds[/color][/size]
The values for Block Rating and Defense Rating that were used for generating most builds are tailored for Eidolon NM and probably other raids (including elite) as well.
We all know that you have to run more hit in Facility and that some bosses have more or less block rating.
Getting the defensive stats of different dungeons/bosses (and verifying other raids) sounds like a valuable addition.

[size=+1][color=#B2EBF2]Better optimizing rotations[/color][/size]
Currently, rotations are only based around the rules that I described earlier.
I'm pretty sure that especially for burst fights, some additional tweaking of the rotation might result in more DPS.

[size=+1][color=#B2EBF2]Affliction-based builds[/color][/size]
I have some afflicts already implemented, but for a full evaluation of affliction-based builds, I'd have to implement more of them and also important passives and signets for them.
If I recall correctly, Mags already hinted that they might change how afflicts work in order to make them competitive.
Should such a patch hit Live, I'm sure, I'll update my program ;)

[size=+1][color=#B2EBF2]10.9.5 Raid Setting[/color][/size]
If desired, I could add a ".9.5 Raid" build type.
The builds will probably almost identical to the QL11 raid setting, but the stat distribution could be interesting.
My gut feeling says that you should remove crit/pen/crit power equally when you're not QL11.

[size=+1][color=#B2EBF2]Improve Optimization Algorithm[/color][/size]
I feel like there is a lot of potential to improve the speed of the optimization and guide it more towards better builds.
This algorithm is based on [Genetic Algorithms](https://en.wikipedia.org/wiki/Genetic_algorithm) and this is a common problem for them as well.
I have some ideas that I could test in my spare time ;)




================================================================================
Build layout (example):

[size=+2][color=#03A9F4]Pistol/Shotgun[/color] (Raid setting, [color=#D32F2F]7500 DPS[/color])[/size]

[size=+1][color=#B2EBF2]Build[/color][/size]
[table]
[tr][th]Skill[/th][th]Augment[/th][th]Passive[/th][/tr]
[tr][td][color=#03A9F4][b]Hair Trigger[/b][/color] [color=#B6B6B6][i](+7.43% dps)[/i][/color][/td][td]with [color=red][b]Fierce[/b][/color] [color=#B6B6B6][i](+1.54% dps)[/i][/color][/td][td][color=#03A9F4][b]Sudden Return[/b][/color] [color=#B6B6B6][i](+7.43% dps)[/i][/color][/td][/tr]
[tr][td][color=#03A9F4][b]Hair Trigger[/b][/color] [color=#B6B6B6][i](+7.43% dps)[/i][/color][/td][td]with [color=#FFEB3B][b]Mercurial[/b][/color] [color=#B6B6B6][i](+1.54% dps)[/i][/color][/td][td][color=#03A9F4][b]Sudden Return[/b][/color] [color=#B6B6B6][i](+7.43% dps)[/i][/color][/td][/tr]
[/table]

Gear:
Slot - Glyphs - Signets - Notes
Head - 407 Hit - Laceration (+9.72% dps)
Neck - 296 CritPower - Woodcutters Wrath (+13.33% dps) - -10.40% dps if QL11 + Violence, -8.50% dps if .9 Egon Pendant
(QL11 + Violence for Coney!)

Stimulant: 342 Crit (+1.05% dps)
Kickback: 644 CritPower on Pen (+3.26% dps)
Potion: 119 CR (+1.1% dps)

Rotation/Usage:
 * Use resources on 5 for Blood
 * Use resources on 2 for Elemental
 * Prefer to consume on buffs (DABS, EF, WC)
 * Force blood offering uptime

Damage By Source:
- 25.59% 'Bloodshot' (9578 hits, 60.50% crits, 61.60% pens)
- 16.54% 'Power Line Detonation' (1536 hits, 61.40% crits, 46.00% pens)
...

Damage By Type:
 - 80.26% Magic
 - 35.25% Blood
 - 25.59% Consumer
...

Stat variance:
Stat -200 -100 -50 +50 ...
Hit -1% -1% ...

Notes:
 * builder weapon first
