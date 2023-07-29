# CMakeLists.txt - Part of NotedELN, (C) Daniel Wagenaar 2021

# NEW BUILD INSTRUCTIONS (as of version 1.3.x)
# Type:
#   cd build
#   cmake ..
# to build ELN.


######################################################################
cmake_minimum_required(VERSION 3.16)

######################################################################
# Set the project name and version
project(NotedELN VERSION 1.5.2)

set(CMAKE_PROJECT_HOMEPAGE_URL "http://danielwagenaar.net/eln")

######################################################################
# Specify the C++ standard [must be done _before_ adding executable!]
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_POSITION_INDEPENDENT_CODE ON) # I thought this would do the trick...
#if (UNIX) 
#  add_compile_options(-fPIC) # ... but in fact, this is required on Linux
#endif()

# Qt stuff
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt6
  REQUIRED COMPONENTS Widgets Network Svg PrintSupport
  OPTIONAL_COMPONENTS Multimedia MultimediaWidgets
  )

######################################################################
# Add the executables
add_executable(notedeln WIN32 MACOSX_BUNDLE src/App/main.cpp)
add_executable(webgrab WIN32 webgrab/webgrabWE.cpp)

include(src/CMakeLists.txt)
include(webgrab/CMakeLists.txt)

# Libraries to link to - Qt must be mentioned
target_link_libraries(notedeln Qt6::Widgets)
target_link_libraries(notedeln Qt6::Network)
target_link_libraries(notedeln Qt6::Svg)
target_link_libraries(notedeln Qt6::PrintSupport)
if(TARGET Qt6::Multimedia)
  target_link_libraries(notedeln Qt6::Multimedia)
endif()
if(TARGET Qt6::MultimediaWidgets)
  target_link_libraries(notedeln Qt6::MultimediaWidgets)
endif()

if (WIN32)
  target_link_libraries(notedeln secur32.lib)
endif()
if (APPLE)
  set(MACOSX_BUNDLE_ICON_FILE eln.icns)
  add_custom_command(TARGET notedeln POST_BUILD
     COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/notedeln.app/Contents/Resources
     COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/src/App/eln.icns
        ${CMAKE_CURRENT_BINARY_DIR}/notedeln.app/Contents/Resources
  )
endif()

# Produce configuration files
configure_file(src/config.h.in config.h)

# Include paths - Qt must be mentioned
target_include_directories(notedeln PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(notedeln PUBLIC "${Qt6_INCLUDE_DIRS}")

######################################################################
# Spinx documentation
if (UNIX AND NOT APPLE)
  add_subdirectory("docs")
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
  include(MacDMG.CMakeLists.txt)
elseif (UNIX)
  include(UnixInstall.CMakeLists.txt)
  include(DebPack.CMakeLists.txt)
endif()

include(CPack)

