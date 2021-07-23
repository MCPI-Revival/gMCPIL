<h1 align="center">gMCPIL</h1>
<p align="center">
	Simple launcher for Minecraft: Pi Edition and <a href="https://gitea.thebrokenrail.com/TheBrokenRail/minecraft-pi-reborn/">MCPI-Reborn</a> - Vala/GTK+ Edition.
</p>
<p align="center">
	<a href="https://github.com/MCPI-Revival/gMCPIL/blob/master/LICENSE">
		<img src="https://img.shields.io/github/license/MCPI-Devs/MCPIL?label=License" alt="GPL-2.0"></img>
	</a>
	<a href="https://github.com/MCPI-Revival/gMCPIL/releases/latest">
		<img src="https://img.shields.io/github/v/release/MCPI-Revival/gMCPIL" alt="Latest release"></img>
	</a>
</p>

<p align="center">
	<img src="https://raw.githubusercontent.com/MCPI-Revival/gMCPIL/master/screenshot.png" alt="screenshot"></img>
</p>

## Getting started
### Prerequisites
 + Debian Buster-based OS or higher
 + [MCPI-Reborn](https://gitea.thebrokenrail.com/TheBrokenRail/minecraft-pi-reborn)

### Installation
You can install gMCPIL from our APT repo, [mcpi-packages](https://github.com/MCPI-Revival/mcpi-packages):
```sh
# If you didn't install mcpi-packages yet
wget -qO- https://raw.githubusercontent.com/MCPI-Revival/mcpi-packages/master/install.sh | bash

# Actual gMCPIL installation
sudo apt install gmcpil
```

You can also download and install it from the [releases](https://github.com/MCPI-Revival/gMCPIL/releases) section.

### Compiling
To build gMCPIL, you'll need Vala, a C compiler, and some additional packages.
Assuming a Debian-based distro, you can install them with the following command:
```sh
# For Vala + GCC
sudo apt install gcc valac

# Libraries and other build dependencies
sudo apt install git make libjson-glib-dev libgtk-3-dev dpkg-dev fakeroot
```

After installing the build dependencies, you can clone the repo and build gMCPIL:
```sh
git clone https://github.com/MCPI-Revival/gMCPIL
cd gMCPIL

make # Build gMCPIL
make pack # Make a Debian package
```

You can also set the `DEBUG` environment variable to add debug symbols to the executable.

## Changelog
See the [CHANGELOG.txt](https://github.com/MCPI-Revival/gMCPIL/blob/master/res/doc/gmcpil/CHANGELOG.txt) file.

## Licensing
All the code of this project is licensed under the [GNU General Public License version 2.0](https://github.com/Alvarito050506/MCPIL/blob/master/LICENSE) (GPL-2.0).

All the documentation of this project is licensed under the [Creative Commons Attribution-ShareAlike 4.0 International](https://creativecommons.org/licenses/by-sa/4.0/) (CC BY-SA 4.0) license.

![CC BY-SA 4.0](https://i.creativecommons.org/l/by-sa/4.0/88x31.png)
