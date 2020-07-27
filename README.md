Resource repo
======================
https://github.com/JackBailey/hl-test-res


HL1 Mod Testing Environment
======================
This repository serves as a main hub of sorts for any of my Half-Life mods. I'll be using this code as a base for all my future mods, and decided that it'd be nice to release it, since I try to fix any bugs in the vanilla code I can find, as well as add features that I personally think might be useful for general modding purposes.

You're free to pick out any code snippets you want and use them in whatever way you want to. Credit would be nice, but isn't required in this case.

However, if you choose to use this base in it's entirety, then you have to credit me, as well as have the source code for the mod that uses it publicly available.


Feature List (incomplete)
======================
General:
- Dynamically adjustable model scaling
- Added change-able blood effects
- Added adjustable pain screen flash
- Added "advanced" options
- Fixed raw mouse input

Monsters:
- Collision is off as soon as they die
- Lowered stuck delay
- Re-implemented dead body "physics"
- Added monster_hassault

Zombies:
- Unless specified otherwise through the map, zombies will randomize between scientists, barneys, or soldiers on spawn

Weapons:
- Completely removed client-side implementation; they're now fully server-side
- Reloading, switching weapons, etc. doesn't stop you from using stuff anymore
- Changed ammo counts
- Added weapon_50cal
- Added ammo_50cal
- Added weapon_deagle
- Added weapon_wrench
- Added weapon_knife
- Extended bullet casing lifetime

Crowbar:
- Changed and unified attack delay
- Added viewpunch

MP5:
- Changed attack delay
- Changed viewpunch amount on both primary and secondary attacks
- Removed multiplayer spread

Shotgun:
- New shell ejection handling, now it actually works properly
- Cocking sound now handled by model
- Removed multiplayer spread
- Changed attack and reload delays


To-do:
======================
- Add Bass support for music playing and *maybe* replace the vanilla audio system (don't forget about FLAC, MIDI and FX plugins)
- Add weapon_wrench alt fire
- Add monster_panthereye
- Add monster_gonome
- Add "Explosive" spawnflag to monster_houndeye
- Add "Fast" and "Strong" spawnflags to monster_alien_slave


Credits & Special Thanks
======================
James - Teaching me almost everything I know about HL1 modding and minor code snippets

The Spirit of Half-Life Team - Code snippet for customizable model scaling

Magic Nipples - HAssault code (from his Monster Files series) & server-side weapon implementation

Solokiller - Bugfixing

ZooL - Raw mouse input fix (from Half-Nuked)

Jane - Motivating me to make stuff

Charlie M. - Being an asshole and annoying the shit outta me on Discord while I was trying to work

Sayok & Programme - Being awesome friends and helping me out a lot


Sam H. - Being there when I needed him the most. I love you.


Half Life 1 SDK LICENSE
======================

Half Life 1 SDK Copyright© Valve Corp.  

THIS DOCUMENT DESCRIBES A CONTRACT BETWEEN YOU AND VALVE CORPORATION (“Valve”).  PLEASE READ IT BEFORE DOWNLOADING OR USING THE HALF LIFE 1 SDK (“SDK”). BY DOWNLOADING AND/OR USING THE SOURCE ENGINE SDK YOU ACCEPT THIS LICENSE. IF YOU DO NOT AGREE TO THE TERMS OF THIS LICENSE PLEASE DON’T DOWNLOAD OR USE THE SDK.

You may, free of charge, download and use the SDK to develop a modified Valve game running on the Half-Life engine.  You may distribute your modified Valve game in source and object code form, but only for free. Terms of use for Valve games are found in the Steam Subscriber Agreement located here: http://store.steampowered.com/subscriber_agreement/ 

You may copy, modify, and distribute the SDK and any modifications you make to the SDK in source and object code form, but only for free.  Any distribution of this SDK must include this license.txt and third_party_licenses.txt.  
 
Any distribution of the SDK or a substantial portion of the SDK must include the above copyright notice and the following: 

DISCLAIMER OF WARRANTIES.  THE SOURCE SDK AND ANY OTHER MATERIAL DOWNLOADED BY LICENSEE IS PROVIDED “AS IS”.  VALVE AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES WITH RESPECT TO THE SDK, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT, TITLE AND FITNESS FOR A PARTICULAR PURPOSE.  

LIMITATION OF LIABILITY.  IN NO EVENT SHALL VALVE OR ITS SUPPLIERS BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR INABILITY TO USE THE ENGINE AND/OR THE SDK, EVEN IF VALVE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.  
 
 
If you would like to use the SDK for a commercial purpose, please contact Valve at sourceengine@valvesoftware.com.


Half-Life 1
======================

This is the README for the Half-Life 1 engine and its associated games.

Please use this repository to report bugs and feature requests for Half-Life 1 related products.

Reporting Issues
----------------

If you encounter an issue while using Half-Life 1 games, first search the [issue list](https://github.com/ValveSoftware/halflife/issues) to see if it has already been reported. Include closed issues in your search.

If it has not been reported, create a new issue with at least the following information:

- a short, descriptive title;
- a detailed description of the issue, including any output from the command line;
- steps for reproducing the issue;
- your system information.\*; and
- the `version` output from the in‐game console.

Please place logs either in a code block (press `M` in your browser for a GFM cheat sheet) or a [gist](https://gist.github.com).

\* The preferred and easiest way to get this information is from Steam's Hardware Information viewer from the menu (`Help -> System Information`). Once your information appears: right-click within the dialog, choose `Select All`, right-click again, and then choose `Copy`. Paste this information into your report, preferably in a code block.

Conduct
-------


There are basic rules of conduct that should be followed at all times by everyone participating in the discussions.  While this is generally a relaxed environment, please remember the following:

- Do not insult, harass, or demean anyone.
- Do not intentionally multi-post an issue.
- Do not use ALL CAPS when creating an issue report.
- Do not repeatedly update an open issue remarking that the issue persists.

Remember: Just because the issue you reported was reported here does not mean that it is an issue with Half-Life.  As well, should your issue not be resolved immediately, it does not mean that a resolution is not being researched or tested.  Patience is always appreciated.
