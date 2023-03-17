# NotedELN - an Electronic Lab Notebook

## Introduction

NotedELN is an Electronic Lab Notebook for scientists of all stripes. 
NotedELN is great for:

 *  Taking notes during experiments since it automatically saves your
    input every 10 seconds so that your notes will never be lost;

 *  Keeping track of data analysis results since screen shots and
    other graphics can be added to a notebook page by drag-and-drop or
    copy-and-paste;

 *  Recording your lit searches, since it will automatically download
    papers from PubMed and archive them with your notebook;

 *  Recording your thought processes, since it will never get in the
    way of your typing with arbitrary constraints on input formatting.

NotedELN is quite mature and used daily in our lab. NotedELN has never 
caused anyone to lose any data or notes. To the contrary, since most 
people type faster than they can write by hand, ELN has helped people 
take more detailed notes than they would otherwise have.

NotedELN is free and open source and is available for Windows, Mac, and
Linux.

Development continues, and your suggestions, bug reports, and
contributions are welcome.

## Installation

On Ubuntu Linux, Windows 10, and Mac OS X, you should be able to
install the executables available from [github](https://github.com/wagenadl/notedeln/releases/latest).

## Installation from source on Linux

On most Linux distributions, compilation from source should be
straightforward:

* Make sure you have the required dependencies. Most importantly, you
  should have the complete Qt 5 development system installed, version
  5.6.1 or above. Exact package names vary across distributions.

  On Ubuntu, you would do:

        sudo apt install git qtmultimedia5-dev \
             libqt5webkit5-dev libqt5svg5-dev \
			 latexmk texlive-latex-base texlive-latex-recommended

  On OpenSUSE, you would do:

        sudo zypper install --no-recommends git libqt5-qtbase-common-devel \
             libqt5-qtmultimedia-devel libQt5WebKitWidgets-devel \
             libqt5-qtsvg-devel libqt5-qttools-devel \
             libQt5PrintSupport-devel \
			 latexmk texlive-latex texlive-latex-recommended

  (I would be happy to include your instructions for other
  distributions. Please [drop me a line](mailto:daw@caltech.edu).)

* Clone the NotedELN code from github:

        git clone https://github.com/wagenadl/eln.git

* Enter the eln directory:

        cd eln

* Compile the code:

        mkdir build
        cd build
        cmake ..
        cmake --build .

* Test the result:

        ./notedeln

* Once you are happy, install NotedELN to a system location:

        sudo cmake --install .

  (This installs the binaries in `/usr/local/bin` and some extra files in
  `/usr/local/share`. If you prefer other locations, use the --prefix
  option to cmake.)
  
* If you are on Debian or a derived system, you can also create a ".deb"
  by typing
  
        cpack
		
  The resulting "notedeln-xxx.deb" can be installed with "dpkg -i" in the
  usual manner.

Please let me know if you have trouble. I will gladly try to help.

## Installation from source on Mac OS X

It's been a little while since I last built NotedELN on Mac OS. In principle, 
CMake should make it a breeze, but let me know if you run into trouble.

* You need to get the Qt development system and Apple's XCode
  development tools.

* After that, the same steps should compile the code:

        git clone https://github.com/wagenadl/notedeln.git
        cd notedeln
        mkdir build
        cd build
        cmake ..
        cmake --build .

* You can then simply run NotedELN from within the build tree, or—more
  conveniently—build a package:

        cpack

  That should generate a file called "notedeln.dmg" that you can install
  in the usual way.

## Installation from source on Windows

Building the source on Windows is easier than it once was, thanks to CMake. Most importantly,
you have to pay attention to the Qt installation path. 

* You need to get Visual Studio (Community Edition works great) and the Qt development system,
  version 5.12 or later. (NotedELN has not yet been tested with Qt 6.)

* If you want to build an installation package, you will need 
  [NSIS](https://nsis.sourceforge.io/Download).

* Open a "git bash" terminal in some appropriate location, then type:

        git clone https://github.com/wagenadl/notedeln.git
		
  to download the repository.
  
* Continue typing in the terminal to build the package:

        cd notedeln
        mkdir build
        cd build
        export CMAKE_PREFIX_PATH=/c/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5 
        export PATH=/c/Qt/5.15.2/msvc2019_64/bin:$PATH
        cmake ..
        cmake --build . --config Release
		
  You will have to change the path to Qt if you have a different version.
  Caution: Don't assume all is well if it builds without specifying a path. I have seen CMake using one 
  version of Qt for the actual building and an other for collecting DLLs for deployment. If you 
  change the CMAKE_PREFIX_PATH or PATH during experimentation, you must clean out the "build" folder
  entirely before rebuilding.

* You can now run the notedeln binary in-place, but you probably want to create an installation 
  package, so keep typing in the terminal:

        cpack

* This results in a file called NotedELN-x.y.z-win64.exe in the "build" folder.
  Double click it to install.
	
* Before installing, you may check that \_CPack\_Packages\win64\NSIS/NotedELN-x.y.z\bin\notedeln.exe actually runs.
