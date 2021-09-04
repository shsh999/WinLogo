# WinLogo
## About
This project is used to modify the start button logo of windows 10 computers.

As for now, the logos are precompiled into the binary, and replacing them requires a recompilation (or changing the resources in a DLL manually). For more information, see [Logo Images](#Logo-Images).

![Installer UI](/docs/ui.png)
![Taskbar](/docs/taskbar.jpg)

### Supported Versions
This tool was tested on Windows 10 21h1, but should work for any "new enough" version of Windows 10.

## Installation
Currently, the installation can only be global for all the users in the computer, and requires administrative privileges.

### Using the UI
Just slide the UI slider to install / uninstall. It may take a few seconds.

### Manually
For a scripted deployment, you may do the same things as the UI manually:

#### Install
1. Put winlogo.dll in a globally available directory (such as `C:\Windows\System32`)
2. In cmd, run `regsvr32.exe winlogo.dll`

#### Uninstall
1. In cmd, run `regsvr32.exe /u winlogo.dll`
2. Restart explorer.exe
3. Delete the dll

## Compiling
The tool is built using Visual Studio 2019 - just use it to open the .sln file and compile (you might have to install the nuget dependencies for the UI project).

## How Does it Work?
### Registration
The tool is a DLL (`winlogo.dll`) loaded into explorer. In order to get loaded, it is registered as an Icon Overlay (that's why it can only be registered globally for all users).

For more details about the registration process, see [Registration.cpp](winlogo\Registration.cpp).

### Painting
WinLogo uses IAT hooking to hook several functions used by explorer while drawing the start button (See [LogoHooks.cpp](winlogo\LogoHooks.cpp)):
* `OpenThemeDataForDpi` - When explorer loads the theme, the theme handle for "Taskbarpearl" is saved in order to later find out when explorer draws the start button.
* `CloseThemeData` - Unregisters the theme handle when it's closed.
* `DrawThemeBackground` - Skips the drawing of the start button. The next hook is responsible for the drawing. The function is only called when drawing a "disabled" logo, so it is used to mark which type of logo we should draw.
* `EndBufferedPaint` - Winlogo hijacks this function in order to draw the actual logo images.

### Logo Images
The logo images are taken from resources compiled into the binary, named `enabled.bmp` and `disabled.bmp`.
Currently, there are several limitations regarding the images:
* They are compiled directly into winlogo.dll
* They must be BMP files
* Transparency is achieved by using a black background
* The size is used for the drawing - there is now stretching / resizing done by winlogo.

## Credits
### GSL
The code uses the Guidelines Support Library, as taken from [this repository](https://github.com/microsoft/GSL), commit 10ce5b634f40d1a51c7ab92c362c77d4438c1d51.

### RestartManagerSession
The code for RestartManagerSession.cs was taken from the following gist: https://gist.github.com/falahati/34b23831733151460de1368c5fba8e93.