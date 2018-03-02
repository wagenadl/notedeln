# ELN - an Electronic Lab Notebook

## Introduction

ELN is an Electronic Lab Notebook for scientists of all stripes. ELN
is great for:

 *  Taking notes during experiments since it automatically saves your
    input every 10 seconds so that your notes will never be lost;

 *  Keeping track of data analysis results since screen shots and
    other graphics can be added to a notebook page by drag-and-drop or
    copy-and-paste;

 *  Recording your lit searches, since it will automatically download
    papers from PubMed and archive them with your notebook;

 *  Recording your thought processes, since it will never get in the
    way of your typing with arbitrary constraints on input formatting.

ELN is fairly mature and used daily in our lab. ELN has never caused
anyone to lose any data or notes. To the contrary, since most people
type faster than they can write by hand, ELN has helped people take
more detailed notes than they would otherwise have.

ELN is free and open source and is available for Windows, Mac, and
Linux. Please visit its [web page](http://www.danielwagenaar.net/eln)
for download details.

Development continues, and your suggestions, bug reports, and
contributions are welcome.

## Installation

On Ubuntu Linux, Windows 7/10, and Mac OS X, you should be able to
install the executables available from
http://www.danielwagenaar.net/eln.html.

## Installation from source on Linux

On most Linux distributions, compilation from source should be
straightforward:

* Make sure you have the required dependencies. Most importantly, you
  should have the complete Qt development system installed, version
  5.6.1 or above. Packages vary widely across distributions.

  On Ubuntu, you would do:

        sudo apt install git qtmultimedia5-dev \
             libqt5webkit5-dev libqt5svg5-dev asciidoc

  On OpenSUSE, you would do:

        sudo zypper install --no-recommends git libqt5-qtbase-common-devel \
             libqt5-qtmultimedia-devel libQt5WebKitWidgets-devel \
             libqt5-qtsvg-devel libqt5-qttools-devel \
             libQt5PrintSupport-devel asciidoc

  (I would be happy to include your instructions for other
  distributions. Please [drop me a line](mailto:daw@caltech.edu).)

* Clone the ELN code from github:

        git clone https://github.com/wagenadl/eln.git

* Enter the eln directory:

        cd eln

* Compile the code:

        make

* Test the result:

        ./build/eln

* Once you are happy, install ELN to a system location:

        sudo make install

  (This installs the binaries in `/usr/local/bin` and some extra files in
  `/usr/local/share`. If you prefer other locations, please edit lines 10
  and 11 of the Makefile.)

Please let me know if you have trouble. I will gladly try to help.

## Installation from source on Mac OS X

Building from source on Mac is only slightly more involved.

* You need to get the Qt development system and Apple's XCode
  development tools.

* After that, the same `git clone` / `cd eln` / `make` steps
  should compile the code:

        git clone https://github.com/wagenadl/eln.git
        cd eln
        make

* You can then simply run ELN from within the build tree, or—more
  conveniently—build a package:

        make macdmg

  That should generate a file called "eln.dmg" that you can install
  in the usual way.

## Installation from source on Windows

In my experience, this tends to be somewhat tricky.

* You need to get the Qt development system.

* If you want to build an installation package, you will need [Inno
  Setup](http://www.jrsoftware.org/isinfo.php).

* Use the Qt Creator to build and compile "eln" and "webgrab" in "Release" mode.

* Test the resulting executable.

* To properly install:

  * Open a bash shell (from git or cygwin) in the ELN root directory and type

            ./tools/windeploy.pl

      If you get errors, you will need to edit that script to correct paths to Qt
      and other essential components.

  * Open `eln-x86.iss` in Inno Setup; compile and run.
