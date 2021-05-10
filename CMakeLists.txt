# CMakeLists.txt - Part of ELN, (C) Daniel Wagenaar 2021

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
add_executable(noted WIN32 MACOSX_BUNDLE)
add_executable(webgrab WIN32 MACOSX_BUNDLE)

include(src/CMakeLists.txt)
include(webgrab/CMakeLists.txt)

# Libraries to link to - Qt must be mentioned
target_link_libraries(noted Qt5::Widgets)
target_link_libraries(noted Qt5::Network)
target_link_libraries(noted Qt5::Svg)
target_link_libraries(noted Qt5::PrintSupport)

# Produce configuration files
configure_file(src/config.h.in config.h)
configure_file(doc/version.tex.in version.tex)

# Include paths - Qt must be mentioned
target_include_directories(noted PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(noted PUBLIC "${Qt5Widgets_INCLUDE_DIRS}")

######################################################################
# Userguide
include(doc/UseLATEX.cmake)
add_latex_document(doc/userguide.tex IMAGE_DIRS doc doc/icons
  FORCE_PDF INCLUDE_DIRECTORIES doc)
######################################################################
# Installation
include(Install.CMakeLists.txt)

######################################################################
# General packaging
set(CPACK_PACKAGE_VENDOR "Daniel Wagenaar")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Noted ELN - an Electronic Lab Notebook")
set(CPACK_PACKAGE_DESCRIPTION
  "Noted ELN is an Electronic Lab Notebook that lets you focus on note taking.
Noted ELN supports text, images, and basic graphical annotations. 
Noted ELN makes safeguarding your notebook entries its number one priority 
and is extremely stable.")
set(CPACK_PACKAGE_EXECUTABLES "noted;Noted ELN")

# OS dependent packaging choices
if (WIN32)
  set(CPACK_GENERATOR "NSIS")
elseif(APPLE)
  set(CPACK_GENERATOR "DMG") # NYI
elseif(UNIX)
  set(CPACK_GENERATOR "DEB")
endif()

# DEB packaging - to be updated
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Daniel A. Wagenaar <daw@caltech.edu>")
set(CPACK_DEBIAN_PACKAGE_RELEASE "1.2.21")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
# Use my "concise-deps" to figure out minimal list of dependencies
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5widgets5 (>= 5.12), libstdc++6 (>=10.0)")
set(CPACK_DEBIAN_PACKAGE_SECTION "scientific")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "${PROJECT_HOMEPAGE_URL}")

if (WINDOWS)
  # NSIS packaging - to be updated
  include("Windeployqt.cmake")
  windeployqt("chello" "bin")
  set(CPACK_GENERATOR "NSIS")
  set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON")
endif()

if (APPLE)
  get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
  get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
  find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
  add_custom_command(TARGET myapp POST_BUILD
      COMMAND "${MACDEPLOYQT_EXECUTABLE}"
          "$<TARGET_FILE_DIR:myapp>/../.."
          -always-overwrite
      COMMENT "Running macdeployqt..."
  )
endif()

# Packaging
include(CPack)

