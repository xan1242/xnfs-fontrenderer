# XNFS-FontRenderer
A new (vector) font renderer for NFS games

Currently VERY limited (no scaling & rotation).

## Game compatibility
Currently only working in:
- Need for Speed: Most Wanted
- Need for Speed: Carbon

After a little bit of research, it is expected to also work in other titles, specifically:
- Need for Speed: Underground
- Need for Speed: Underground 2
- Need for Speed: ProStreet

Undercover and newer do not use FrontEnd (except for HUD in Undercover), therefore this code will not apply to those games.

## Building
The included solution is made from VS2015, however, you should be able to build this with any Visual Studio or Dev C++.

- Open XNFS-FontRenderer.sln
- Select the configuration of the game that you desire.
- Output files will be in the configuration's folder name.
- Copy the .asi and .ini files to your scripts folder of the game (requires an ASI loader)
- (optional) If running Carbon, install CarbonTitle.otf (included) and Pragmatica Condensed to your system.
- (optional) If running Most Wanted, install Microsquare to your system.

# CarbonTitle font
This is a custom retrace of the Carbon's unique title font. Currently, only the numbers are traced.

Project files are made with FontForge.
