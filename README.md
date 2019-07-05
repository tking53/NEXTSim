# NEXTSim README

[Cory R. Thornsberry](https://github.com/cthornsb)

Last updated: July 3, 2019

This README gives basic information on how to install NEXTSim from source. For more information
on using the program, see the [NEXTSim Wiki](https://github.com/cthornsb/NEXTSim/wiki). For detailed
information on changes to the master branch, see the [changelog](https://github.com/cthornsb/NEXTSim/wiki/Changelog).

## Authors

- Cory R Thornsberry
- David P&eacute;rez Loureiro 
- Kyle Schmitt
- Xiadong Zhang

## Installation

- Required Prerequisites:
	- [cmake](https://cmake.org/) version 2.18.12 or later is confirmed to work
	- [Geant4](https://geant4.web.cern.ch/support/download) version 10.3.3 or later is recommended
		- Build option `-DGEANT4_BUILD_MULTITHREADED=ON` is required for multithreading support
		- Build option `-DGEANT4_USE_GDML=ON` is required for GDML support
	- [ROOT](https://root.cern.ch/downloading-root) version 6 or later is recommended

- Recommended Prerequisites:
	- [ccmake](https://cmake.org/cmake/help/latest/manual/ccmake.1.html)
		- The command `sudo apt-get install cmake-curses-gui` works on Ubuntu
	- [Environment Modules](http://modules.sourceforge.net/)
	
- Optional Prerequisites:
	- [SimpleScan](https://github.com/cthornsb/SimplePixieScan) needed for nextSim output converter

```bash
git clone https://github.com/cthornsb/NEXTSim
cd NEXTSim
mkdir build
cd build
```

Common NEXTSim install options are shown in the table below

|OPTION                  | DEFAULT | DESCRIPTION |
|------------------------|---------|-------------|
|CMAKE\_INSTALL\_PREFIX  | install | Set the install prefix
|GEANT4\_MT              | OFF     | Enable or disable multithreading capability
|GEANT4\_GDML            | OFF     | Enable or disable support for loading GDML files
|GEANT4\_UIVIS           | ON      | Enable or disable Geant4 UI and VIS drivers
|BUILD\_TOOLS            | OFF     | Enable tool executables
|BUILD\_TOOLS\_NISTLIST  | OFF     | Build NIST database search executable
|BUILD\_TOOLS\_CONVERTER | OFF     | Build NEXTSim -> SimpleScan converter tool
|BUILD\_TOOLS\_CMDSEARCH | OFF     | Build NEXTSim macro command search executable

Install options may be set using ccmake e.g.

```bash
cmake ..
ccmake ..
```

or by passing options to CMake using the command line e.g.

```
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install -DBUILD_TOOLS=ON ..
```

and so forth. Once CMake has successfully generated the makefile,
simply type


```bash
make install
```

to compile and install. Experimental/unstable beta versions of 
the simulation code may be found on the development branch

```bash
git checkout dev
cd build
make install
```

To switch back to the master release,

```bash
git checkout master
cd build
make install
```

### Installing the Module File

By default, cmake will install binaries to

NEXTSim/install/bin

From there you may make symbolic links as required, or if you
prefer you may use the NEXTSim module file

To installing the NEXTSim module file, the module file must
be copied to one of your module paths.

```bash
cp install/share/modulefiles/nextSim ${PATH_TO_MODULE_FILES}
```

Where PATH\_TO\_MODULE\_FILES is dependent upon the configuration
of your machine. You can check for the path by typing

```bash
module avail
```

For example, on my machine, the syntax would be

```bash
cp install/share/modulefiles/nextSim /usr/share/modules/modulefiles/
```

Whenever you want to use NEXTSim, load the NEXTSim module by typing

```bash
module load nextSim
```

### Updating the Source Code

In the ./NEXTSim/ directory type

```bash
git pull
cd build/
make install
```

Barring a substantial change in the structure of the source code,
this should be sufficient for keeping your install up-to-date with
the repository.
