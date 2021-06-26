# Vanilla Conquer port for PS Vita

## Install
Download vanillatd.vpk or vanillara.vpk file from https://github.com/Northfear/Vanilla-Conquer-vita/releases. Install it to your PS Vita.

Copy content of installed Tiberium Dawn game folder into to ```ux0:data/VanillaTD/``` or installed Red Alert folder into ```ux0:data/VanillaRA/```.

Check Vanilla Conquer Wiki for more info about required folder/file structure and game versions compatibility:

[Installing VanillaTD](https://github.com/TheAssemblyArmada/Vanilla-Conquer/wiki/Installing-VanillaTD)

[Installing VanillaRA](https://github.com/TheAssemblyArmada/Vanilla-Conquer/wiki/Installing-VanillaRA)

```expand.mix, expand2.mix, hires1.mix``` files that are required for RA expansions can be aquired from 3.03 patch.

[rePatch reDux0](https://github.com/dots-tb/rePatch-reDux0) OR [FdFix](https://github.com/TheOfficialFloW/FdFix) plugin may be required for proper suspend/resume support (only use one at a time).

## Building

### Prerequisites
- VitaSDK
- SDL2
- OpenAL

### Build
```
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake -DVITA=true -DCMAKE_BUILD_TYPE=None
make
```

Debug output can be previewed with psp2shell

https://github.com/Cpasjuste/psp2shell

## Port info

### Controls

- Left analog stick - Cursor movement / Map scrolling (switch between both modes with a 3-fingress press on the rear touchpad)
- Right analog stick - Map scrolling
- CROSS - Left mouse button
- CIRCLE - Right mouse button (Cancel building, deselect unit..)
- SQUARE - G (Guard Area)
- TRIANGLE - F (Formation. RA only)
- D-Pad Up/Right/Down/Left - 1/2/3/4 button
- R1 - Alt (force move)
- L1 - Ctrl (force attack)
- SELECT - Esc (opens menu, skips videos)
- START - Enter (to submit score after the mission)

Use R1 + D-Pad to create teams (1-4) and D-Pad to select them (same as Ctrl + 1-4 on keyboard). You can use DPad numbers while entering savegame names.

Left analog stick can be used for both cursor movement (good for building or precise movement) or map scrolling (useful during combat when you're giving command with a touchpad). Cursor movement mode is default. To switch between both modes use R1 + L1 + START key combination.

You can change cursor movement speed by editing ```ControllerPointerSpeed``` option under ```[Vita]``` section inside ```conquer.ini```/```redalert.ini``` files.

### Other

Config files for VanillaTD and VanillaRA are located at ```ux0:data/VanillaTD/vanillatd/conquer.ini``` and ```ux0:data/VanillaRA/vanillara/redalert.ini```.

To preserve original aspect ratio set ```Boxing=yes``` option.

If you want to display the game at native resolution without any scaling set ```ScaleGameSurface=no``` (```[Vita]``` section).

Game supports nearest and linear filtering. Nearest is used by default and it produces sharp, but pixelated image (that's especially noticeable on text). Linear is smooth, but somewhat blurred. To select linear filtering set ```Scaler``` option to ```linear```

```
[Video]
Scaler=linear
```

Change it back to ```nearest``` to select nearest filtering again.


# Vanilla Conquer
Vanilla Conquer is a fully portable version of the first generation C&C engine and is capable of running both Tiberian Dawn and Red Alert on multiple platforms. It can also be used for mod development for the Remastered Collection.

The main focus of Vanilla Conquer is to keep the default out-of-box experience faithful to what the games were back when they were released and work as a drop-in replacement for the original executables while also providing bug fixes, compatiblity and quality of life improvements.

Current project goals are tracked as [GitHub issues with the goal label](https://github.com/Vanilla-Conquer/Vanilla-Conquer/issues?q=is%3Aissue+is%3Aopen+label%3Agoal).

Developers hang around [The Assembly Armada Discord server](https://discord.gg/UnWK2Tw) if you feel like chatting.

## Building

We support wide variety of compilers and platforms to target. Vanilla Conquer is known to compile with recent enough gcc, MSVC, mingw-w64 or clang and known to run on Windows, Linux, macOS and BSDs.

### Windows

#### Requirements

The following components are needed to build Vanilla Conquer executables:

 - [MSVC v142 C++ x86/x64 build tools](https://visualstudio.microsoft.com/visual-cpp-build-tools/)
 - Windows 10 SDK
 - CMake (installable from MSVC build tools)
 - [SDL2 development libraries, Visual C++](https://libsdl.org/download-2.0.php)
 - [OpenAL Core SDK](https://www.openal.org/downloads/)

Extract SDL2 and OpenAL somewhere you know. If you are building only Remastered dlls you can skip installing SDL2 and OpenAL.

#### Building

In a VS command line window:

```sh
mkdir build
cd build
cmake .. -DSDL2_ROOT_DIR=C:\path\to\SDL2 -DOPENAL_ROOT=C:\path\to\OpenAL
cmake --build .
```

This will build Vanilla Conquer executables in the build directory. If you are building Remastered dlls you need to configure cmake with `-A win32` and ensure your VS command line is x86.

### Linux / macOS / BSD

#### Requirements

- GNU C++ Compiler (g++) or Clang
- CMake
- SDL2
- OpenAL

On Debian/Ubuntu you can install the build requirements as follows:

```
sudo apt-get update
sudo apt-get install g++ cmake libsdl2-dev libopenal-dev
```

#### Building

```sh
mkdir build
cd build
cmake ..
make -j8
```

This will build Vanilla Conquer executables in the build directory.

## Releases

Binary releases of the latest commit are available from [here](https://github.com/TheAssemblyArmada/Vanilla-Conquer/releases/tag/latest), which is updated whenever new code is merged into the main branch.

## Running

### VanillaTD and VanillaRA

Copy the Vanilla executable (`vanillatd.exe` or `vanillara.exe`) to your legacy game directory, on Windows also copy `SDL2.dll` and `OpenAL32.dll`.

For Tiberian Dawn the final freeware Gold CD release ([GDI](https://www.moddb.com/games/cc-gold/downloads/command-conquer-gold-free-game-gdi-iso), [NOD](https://www.moddb.com/games/cc-gold/downloads/command-conquer-gold-free-game-nod-iso)) works fine.

For Red Alert the freeware [CD release](https://web.archive.org/web/20080901183216/http://www.ea.com/redalert/news-detail.jsp?id=62) works fine as well.
The official [Red Alert demo](https://www.moddb.com/games/cc-red-alert/downloads/command-conquer-red-alert-demo) is also fully playable.
The demo supports custom skirmish maps (except interior) and includes one campaign mission for both Allied and Soviet from the retail game.

While it is possible to use the game data from the Remastered Collection, The Ultimate Collection or The First Decade they are currently _not_ supported.
Any repackaged version that you may already have from any unofficial source is _not_ supported.
If you encounter a bug that may be data related like invisible things or crashing when using a certain unit please retest with the retail data first before submitting a bug report.

### Remastered

The build process will produce _Vanilla_TD_ and _Vanilla_RA_ directories in your build directory if you enable them with `-DBUILD_REMASTERTD=ON` and `-DBUILD_REMASTERRA=ON`.
These work as mods for the Remastered Collection.

To manually install a local Remastered mod, launch both games once then head to _My Documents/CnCRemastered/CnCRemastered/Mods_.
You should see _Tiberian\_Dawn_ and _Red\_Alert_ directories.

#### Tiberian Dawn

Copy the _Vanilla_TD_ directory to the _Tiberian\_Dawn_ directory.

The directory structure should look like this:

    My Documents/CnCRemastered/CnCRemastered/Mods/Tiberian_Dawn/Vanilla_TD/Data/TiberianDawn.dll
    My Documents/CnCRemastered/CnCRemastered/Mods/Tiberian_Dawn/Vanilla_TD/ccmod.json
    My Documents/CnCRemastered/CnCRemastered/Mods/Tiberian_Dawn/Vanilla_TD/GameConstants_Mod.xml

You should now see the new mod in the mods list of Tiberian Dawn Remastered.

#### Red Alert

Copy the _Vanilla_RA_ directory to the _Red\_Alert_ directory.

The directory structure should look like this:

    My Documents/CnCRemastered/CnCRemastered/Mods/Red_Alert/Vanilla_RA/Data/RedAlert.dll
    My Documents/CnCRemastered/CnCRemastered/Mods/Red_Alert/Vanilla_RA/ccmod.json

You should now see the new mod in the mods list of Red Alert Remastered.
