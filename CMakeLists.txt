# CMakeLists.txt - Part of NotedELN, (C) Daniel Wagenaar 2021

# NEW BUILD INSTRUCTIONS (as of version 1.3.x)
# Type:
#   cd build
#   cmake ..
# to build ELN.


######################################################################
cmake_minimum_required(VERSION 3.10.0)

######################################################################
# Set the project name and version
project(NotedELN VERSION 1.3.0)
set(CMAKE_PROJECT_HOMEPAGE_URL "http://danielwagenaar.net/eln")

######################################################################
# Specify the C++ standard [must be done _before_ adding executable!]
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # I thought this would do the trick...
if (UNIX) 
  add_compile_options(-fPIC) # ... but in fact, this is required on Linux
endif()

# Qt stuff
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS Widgets Network Svg PrintSupport REQUIRED)

######################################################################
# Add the executables
add_executable(notedeln WIN32 MACOSX_BUNDLE)
add_executable(webgrab WIN32 MACOSX_BUNDLE)

include(src/CMakeLists.txt)
include(webgrab/CMakeLists.txt)

# Libraries to link to - Qt must be mentioned
target_link_libraries(notedeln Qt5::Widgets)
target_link_libraries(notedeln Qt5::Network)
target_link_libraries(notedeln Qt5::Svg)
target_link_libraries(notedeln Qt5::PrintSupport)

set_target_properties(notedeln PROPERTIES LINK_OPTIONS -s)
set_target_properties(webgrab PROPERTIES LINK_OPTIONS -s)

# Produce configuration files
configure_file(src/config.h.in config.h)
configure_file(doc/version.tex.in version.tex)

# Include paths - Qt must be mentioned
target_include_directories(notedeln PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(notedeln PUBLIC "${Qt5Widgets_INCLUDE_DIRS}")

######################################################################
# Userguide
include(doc/UseLATEX.cmake)
add_latex_document(doc/userguide.tex IMAGE_DIRS doc doc/icons
  FORCE_PDF INCLUDE_DIRECTORIES doc)
######################################################################
# Installation
include(UnixInstall.CMakeLists.txt)

######################################################################
# General packaging
set(CPACK_PACKAGE_VENDOR "Daniel Wagenaar")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NotedELN - an Electronic Lab Notebook")
set(CPACK_PACKAGE_DESCRIPTION
  "NotedELN is an Electronic Lab Notebook that lets you focus on note taking.
NotedELN supports text, images, and basic graphical annotations. 
NotedELN makes safeguarding your notebook entries its number one priority 
and is extremely stable.")
set(CPACK_PACKAGE_EXECUTABLES "notedeln;NotedELN")

# OS dependent packaging choices
if (WIN32)
  set(CPACK_GENERATOR "NSIS")
elseif (APPLE)
  set(CPACK_GENERATOR "DMG") # NYI
elseif (UNIX)
  set(CPACK_GENERATOR "DEB")
endif()

if (WINDOWS)
  # NSIS packaging - to be updated
  include("Windeployqt.cmake")
  windeployqt("notedeln" "bin")
  windeployqt("webgrab" "bin")
  set(CPACK_GENERATOR "NSIS")
  set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON")
elseif (APPLE)
  get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
  get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
  find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
  add_custom_command(TARGET myapp POST_BUILD
    COMMAND "${MACDEPLOYQT_EXECUTABLE}"
    "$<TARGET_FILE_DIR:myapp>/../.."
    -always-overwrite
    COMMENT "Running macdeployqt..."
    )
elseif (UNIX)
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Daniel A. Wagenaar <daw@caltech.edu>")
  set(CPACK_DEBIAN_PACKAGE_RELEASE "1")
  set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
  # Use my "concise-deps" to figure out minimal list of dependencies
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5widgets5 (>= 5.12), libqt5svg5 (>= 5.12), libqt5printsupport5 (>= 5.12), libqt5webkit5 (>= 5.12), libstdc++6 (>=10.0)")
  set(CPACK_DEBIAN_PACKAGE_SECTION "editors")
  set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "${PROJECT_HOMEPAGE_URL}")
endif()

# Packaging
include(CPack)

