# CMakeLists.txt - Part of NotedELN, (C) Daniel Wagenaar 2021

# NEW BUILD INSTRUCTIONS (as of version 1.3.x)
# Type:
#   cd build
#   cmake ..
# to build ELN.


######################################################################
cmake_minimum_required(VERSION 3.11.0)

######################################################################
# Set the project name and version
project(NotedELN VERSION 1.4.99.0)
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
find_package(Qt6
  COMPONENTS Widgets Network Svg PrintSupport Multimedia MultimediaWidgets
  REQUIRED)

######################################################################
# Add the executables
add_executable(notedeln WIN32 MACOSX_BUNDLE src/App/main.cpp)
add_executable(webgrab WIN32 MACOSX_BUNDLE webgrab/Options.cpp)

include(src/CMakeLists.txt)
include(webgrab/CMakeLists.txt)

# Libraries to link to - Qt must be mentioned
target_link_libraries(notedeln Qt6::Widgets)
target_link_libraries(notedeln Qt6::Network)
target_link_libraries(notedeln Qt6::Svg)
target_link_libraries(notedeln Qt6::PrintSupport)
target_link_libraries(notedeln Qt6::Multimedia)
target_link_libraries(notedeln Qt6::MultimediaWidgets)

if (WIN32)
  target_link_libraries(notedeln secur32.lib)
endif()

# Produce configuration files
configure_file(src/config.h.in config.h)
configure_file(doc/version.tex.in version.tex)

# Include paths - Qt must be mentioned
target_include_directories(notedeln PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(notedeln PUBLIC "${Qt6Widgets_INCLUDE_DIRS}")

######################################################################
# Userguide
if (UNIX AND NOT APPLE)
  include(doc/UseLATEX.cmake)
  add_latex_document(doc/userguide.tex IMAGE_DIRS doc doc/icons
    FORCE_PDF INCLUDE_DIRECTORIES doc)
endif()

######################################################################
# General packaging / installation
set(CPACK_PACKAGE_VENDOR "Daniel Wagenaar")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NotedELN - an Electronic Lab Notebook")
set(CPACK_PACKAGE_DESCRIPTION
  "NotedELN is an Electronic Lab Notebook that lets you focus on note taking.
NotedELN supports text, images, and basic graphical annotations. 
NotedELN makes safeguarding your notebook entries its number one priority 
and is extremely stable.")
set(CPACK_PACKAGE_EXECUTABLES "notedeln;NotedELN")

if (WIN32)
  # NSIS packaging - to be updated
  include(WinPack.CMakeLists.txt)
  
elseif (APPLE)
  set(CPACK_GENERATOR "DMG") # NYI

  get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
  get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
  find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
  add_custom_command(TARGET myapp POST_BUILD
    COMMAND "${MACDEPLOYQT_EXECUTABLE}"
    "$<TARGET_FILE_DIR:myapp>/../.."
    -always-overwrite
    COMMENT "Running macdeployqt..."
    )
elseif (UNIX)
  include(UnixInstall.CMakeLists.txt)
  include(DebPack.CMakeLists.txt)
endif()

# Packaging
include(CPack)

