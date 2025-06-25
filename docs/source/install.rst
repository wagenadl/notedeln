.. _install:

Installing NotedELN on your computer
=====================================================

Binary installation
-------------------

Most users on Windows and Debian-derived Linux systems (including
Ubuntu, Mint, etc.), should be able to install NotedELN using
the binary files of the most `recent release
<https://github.com/wagenadl/notedeln/releases/latest>`_.

Installation from source
------------------------

Those who wish to use the software on other systems, or simply prefer
to compile from source, may follow these instructions.

Prerequisites
^^^^^^^^^^^^^

To get started, you will need these prerequisites:

- The `Qt libraries <https://www.qt.io>`_ (version 6.2 or later)
- A C++ compiler (e.g., GCC or `Microsoft Visual Studio <https://visualstudio.microsoft.com/vs/community/>`_)
- The `CMake <https://cmake.org>`_ build system
- The `git <https://git-scm.com>`_ version control system

If you are reading this on a Linux system, chances are you can get
all of these through your distribution's package manager. For
instance, on Ubuntu::

     sudo apt install g++ cmake git qt6-tools-dev \
       qt6-webengine-dev qt6-multimedia-dev libqt6svg6-dev qt6-pdf-dev

On Windows you may have to install the software from the websites
provided above.


Downloading the sources
^^^^^^^^^^^^^^^^^^^^^^^

Download the source for NotedELN from `github
<https://github.com/wagenadl/notedeln>`_. Open a terminal of your
choice in a location on your computer of your choice and type::

    git clone https://github.com/wagenadl/notedeln.git

Of course, if you prefer, there are various graphical frontends for
git that you may use instead of the command line.

Compiling the sources
^^^^^^^^^^^^^^^^^^^^^

After downloading, enter into the downloaded folder::

    cd notedeln

Then, let CMake figure out whether it has all prerequisites::

    cmake -S . -B build

(but first see the :ref:`crit-note-win`, below, if you
are using Windows).

Do not ignore any errors at this stage. However, a warning like

.. epigraph::

    *Sphinx not found - not building documentation*

may safely ignored, unless you want to have a copy of this
documentation on your own computer.

Next, compile the software::

    cmake --build build --config Release

(The “--config Release” is only needed on Windows, but is harmless on
Linux.)

.. _crit-note-win:

Critical note for Windows users
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

On Windows, it is particularly important that CMake uses the right
versions of Qt and the C++ compiler. I have seen builds fail because
different versions were used during different steps of the process. To
be safe, it is best to define a few system variables before the first
`cmake` command, by typing::

    export CMAKE_PREFIX_PATH=/c/Qt6/6.4.1/msvc2019_64/lib/cmake/Qt6
    export PATH=/c/Qt6/6.4.1/msvc2019_64/bin:$PATH
    export VCINSTALLDIR="/c/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC"
      
Of course, you may have to adjust those to point to the appropriate
location on your own system.

Creating an installation package
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

After building, you could simply type::

    cd build
    make install

to install the programs in :file:`/usr/local/bin` (or your operating
system's equivalent), but it may be more attractive to create an
installation package::

    cd build
    cpack

On Ubuntu, that creates a :file:`.deb`; on Windows, a
:file:`.exe`. The :file:`.deb` may be installed like::

    sudo dpkg -i notedeln_1.5.0-1_amd64.deb

And the :file:`.exe` may be installed by double-clicking it.    

Closing words
^^^^^^^^^^^^^

If you have any trouble installing NotedELN, please `drop me a
line <https://www.danielwagenaar.net/members.html>`_. I'd be sad to
lose potential users due to resolvable stumbling blocks.


