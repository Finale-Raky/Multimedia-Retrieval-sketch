# Install script for directory: C:/Users/raky/source/repos/pmp-library/src/pmp

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
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/raky/source/repos/pmp-library/build/Debug/pmp.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/raky/source/repos/pmp-library/build/Release/pmp.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/raky/source/repos/pmp-library/build/MinSizeRel/pmp.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/raky/source/repos/pmp-library/build/RelWithDebInfo/pmp.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/raky/source/repos/pmp-library/build/Debug/pmp.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/raky/source/repos/pmp-library/build/Release/pmp.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/raky/source/repos/pmp-library/build/MinSizeRel/pmp.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/raky/source/repos/pmp-library/build/RelWithDebInfo/pmp.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/pmp" TYPE FILE FILES
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./BoundingBox.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./Exceptions.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./MatVec.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./MemoryUsage.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./Properties.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./SurfaceMesh.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./SurfaceMeshIO.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./Timer.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./Types.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/raky/source/repos/pmp-library/build/src/pmp/visualization/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/pmp/algorithms" TYPE FILE FILES
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/BarycentricCoordinates.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/Curvature.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/Decimation.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/DifferentialGeometry.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/DistancePointTriangle.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/Fairing.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/Features.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/Geodesics.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/Heap.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/HoleFilling.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/NormalCone.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/Normals.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/Parameterization.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/Quadric.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/Remeshing.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/Shapes.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/Smoothing.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/Subdivision.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/TriangleKdTree.h"
    "C:/Users/raky/source/repos/pmp-library/src/pmp/./algorithms/Triangulation.h"
    )
endif()

