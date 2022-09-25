#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "pmp" for configuration "MinSizeRel"
set_property(TARGET pmp APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(pmp PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "${_IMPORT_PREFIX}/lib/pmp.lib"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/pmp.dll"
  )

list(APPEND _cmake_import_check_targets pmp )
list(APPEND _cmake_import_check_files_for_pmp "${_IMPORT_PREFIX}/lib/pmp.lib" "${_IMPORT_PREFIX}/bin/pmp.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
