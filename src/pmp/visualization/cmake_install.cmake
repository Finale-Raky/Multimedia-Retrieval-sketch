# Install script for directory: C:/Users/raky/source/repos/pmp-library/src/pmp/visualization

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/pmp-library")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/raky/source/repos/pmp-library/Debug/pmp_vis.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/raky/source/repos/pmp-library/Release/pmp_vis.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/raky/source/repos/pmp-library/MinSizeRel/pmp_vis.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/raky/source/repos/pmp-library/RelWithDebInfo/pmp_vis.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/pmp/visualization" TYPE FILE FILES
    "C:/Users/raky/source/repos/pmp-library/src/pmp/visualization/./ColdWarmTexture.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/visualization/./GL.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/visualization/./MatCapShader.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/visualization/./MeshViewer.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/visualization/./PhongShader.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/visualization/./Shader.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/visualization/./SurfaceMeshGL.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/visualization/./TrackballViewer.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/visualization/./Window.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/visualization/../MatVec.h"
    )
endif()

