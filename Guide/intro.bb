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

[size+2][color=#03A9F4]ArtificialMind's Ultimate Theorycrafted DPS Guide[/color][/size]
[i][color=gray]Last Update: June 27th, 2016 (1.15.2 with Elite Revamp)[/color][/i]


[size+2][color=#03A9F4]TL;DR: all builds can be found at [url]http://www.markdijkema.nl/projects/tswbuildstatistics[/url][/color][/size]
[i][color=gray](Builds by ArtificialMind, Website by DijkeMark)[/color][/i]


Now that that's out of the way, welcome to my end-game theorycrafted DPS guide!

I've spent a considerable amount of time writing a combat simulator for TSW that accurately captures all mechanics and effects required for evaluating single-target DPS.
For that, I've reverse engineered most of the DPS formula.
The next step was to automatically switch any aspect of a build, re-evaluate its DPS and - if higher - update the original build.
With this mechanic, my program is able to automatically find really high DPS builds for every weapon combination under various situations.
The results can be found at the website linked above (thanks again, Mark, for setting up the site ;) ).

The rest of this guide is structured as follows:
* How to Read the Builds
* The DPS Formula
* FAQ and Pieces of Wisdom
* Changelog, Known Issues, Future Work





================================================================================
[size+2][color=#03A9F4]How to Read the Builds[/color][/size]


================================================================================
[size+2][color=#03A9F4]The DPS Formula[/color][/size]

================================================================================
[size+2][color=#03A9F4]FAQ[/color][/size]
[size+2][color=#03A9F4]Pieces of Wisdom[/color][/size]

================================================================================
[size+2][color=#03A9F4]Changelog[/color][/size]
[size+2][color=#03A9F4]Known Issues[/color][/size]
[size+2][color=#03A9F4]Future Work[/color][/size]



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
