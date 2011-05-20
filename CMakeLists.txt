cmake_minimum_required(VERSION 2.8)

# ----------------------------------------------------------
#  Quick macro for printing certain CMAKE variables
# ----------------------------------------------------------
macro(printCMakeValues)
	message("CMAKE_SOURCE_DIR is ${CMAKE_SOURCE_DIR}")
	message("CMAKE_BINARY_DIR is ${CMAKE_BINARY_DIR}")
endmacro(printCMakeValues)
# ----------------------------------------------------------




project(ext3)
add_executable(ext3 Main.cpp)

printCMakeValues()

include(ExternalProject)

# Set the ExternalProject (ep) base directory
# For each external project, the layout should be:
#  ${ep_base}/tmp/<name>
#  ${ep_base}/Stamp/<name>
#  ${ep_base}/Download/<name>
#  ${ep_base}/Source/<name>
#  ${ep_base}/Build/<name>
#  ${ep_base}/Install/<name>

printCMakeValues()

SET(ep_base "${CMAKE_BINARY_DIR}/CMakeExternals")
#SET(ep_base "${CMAKE_BINARY_DIR}")
set_property(DIRECTORY PROPERTY "EP_BASE" ${ep_base})

option(ITK_DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS "This should match the value used for the DCMTK build.  Defaut is ON." ON)

# ------------------------------------------------------------------
# Create a list of the CMake flags that control compiler settings
# ------------------------------------------------------------------
set(COMPILE_FLAGS     	
	CMAKE_C_FLAGS
	CMAKE_C_FLAGS_DEBUG
	CMAKE_C_FLAGS_MINSIZEREL
	CMAKE_C_FLAGS_RELEASE
	CMAKE_C_FLAGS_RELWITHDEBINFO
	CMAKE_CXX_FLAGS
	CMAKE_CXX_FLAGS_DEBUG
	CMAKE_CXX_FLAGS_MINSIZEREL
	CMAKE_CXX_FLAGS_RELEASE
	CMAKE_CXX_FLAGS_RELWITHDEBINFO
)	


# ----------------------------------------------------------
# Create a macro to load an existing DCMTK CMakeCache.txt
# Note:  Macros have the same scope as the caller, 
#        Functions do not
# ----------------------------------------------------------
macro(LoadDCMTKCache dir_name)
    load_cache("${dir_name}/" READ_WITH_PREFIX "ITK_" 
    	DCMTK_SOURCE_DIR 
    	DCMTK_INCLUDE_DIR 
    	DCMTK_BINARY_DIR 
    	DCMTK_MODULES 
    	LIBXML_INCDIR
    	DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS
    	USE_STD_CXX_INCLUDES
    	${COMPILE_FLAGS}
	)
    message("ITK_DCMTK_SOURCE_DIR is ${ITK_DCMTK_SOURCE_DIR}")
    message("ITK_DCMTK_INCLUDE_DIR is ${ITK_DCMTK_INCLUDE_DIR}")
    message("ITK_DCMTK_BINARY_DIR is ${ITK_DCMTK_BINARY_DIR}")
    message("ITK_DCMTK_DCMTK_MODULES is ${ITK_DCMTK_MODULES}")
    message("ITK_DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS is ${ITK_DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS}")
    message("ITK_USE_STD_CXX_INCLUDES is ${ITK_USE_STD_CXX_INCLUDES}" )
    
    # Save ITK_DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS to the cache
    set(ITK_DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS ${ITK_DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS} CACHE BOOL "This should match the value used for the DCMTK build.  Defaut is ON." FORCE)
    
    message("ITK_CMAKE_CXX_FLAGS_DEBUG is ${ITK_CMAKE_CXX_FLAGS_DEBUG}")
    
	### The CMAKE flags from DCMTK's cache are not reliable because the modified values are not cached ###
    # Create local copies of the DCMTK CMake flags
    #foreach(flag ${COMPILE_FLAGS})
    #	set(${flag} ${ITK_${flag}} CACHE STRING "CMAKE value imported from DCMTK cache" FORCE)
    #endforeach(flag)
endmacro(LoadDCMTKCache)
# ----------------------------------------------------------



# ----------------------------------------------------------
#  This macro will inject the compiler flags that are set 
#  when building dcmtk from source code.
# ----------------------------------------------------------
macro(mimic_dcmtk_compiler_flags)
#IF(ITK_DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS AND NOT WITH_SHARED_LIBS)
IF(ITK_DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS)
  # settings for Microsoft Visual Studio
  IF(CMAKE_GENERATOR MATCHES "Visual Studio .*")
    # get Visual Studio Version
    STRING(REGEX REPLACE "Visual Studio ([0-9]+).*" "\\1" VS_VERSION "${CMAKE_GENERATOR}")
    # these settings never change even for C or C++
    message("CMAKE_C_FLAGS_DEBUG before change = ${CMAKE_C_FLAGS_DEBUG}")
    SET(CMAKE_C_FLAGS_DEBUG "/MTd /Z7 /Od")
    message("CMAKE_C_FLAGS_DEBUG after change = ${CMAKE_C_FLAGS_DEBUG}")
    SET(CMAKE_C_FLAGS_RELEASE "/MT /O2")
    SET(CMAKE_C_FLAGS_MINSIZEREL "/MT /O2")
    SET(CMAKE_C_FLAGS_RELWITHDEBINFO "/MTd /Z7 /Od")
    SET(CMAKE_CXX_FLAGS_DEBUG "/MTd /Z7 /Od")
    SET(CMAKE_CXX_FLAGS_RELEASE "/MT /O2")
    SET(CMAKE_CXX_FLAGS_MINSIZEREL "/MT /O2")
    SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MTd /Z7 /Od")
    # specific settings for the various Visual Studio versions
    IF(VS_VERSION EQUAL 6)
      SET(CMAKE_C_FLAGS "/nologo /W3 /GX /Gy /YX")
      SET(CMAKE_CXX_FLAGS "/nologo /W3 /GX /Gy /YX /Zm500") # /Zm500 increments heap size which is needed on some system to compile templates in dcmimgle
    ENDIF(VS_VERSION EQUAL 6)
    IF(VS_VERSION EQUAL 7)
      SET(CMAKE_C_FLAGS "/nologo /W3 /Gy")
      SET(CMAKE_CXX_FLAGS "/nologo /W3 /Gy")
    ENDIF(VS_VERSION EQUAL 7)
    IF(VS_VERSION GREATER 7)
      SET(CMAKE_C_FLAGS "/nologo /W3 /Gy /EHsc")
      SET(CMAKE_CXX_FLAGS "/nologo /W3 /Gy /EHsc")
    ENDIF(VS_VERSION GREATER 7)
  ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio .*")

  # settings for Borland C++
  IF(CMAKE_GENERATOR MATCHES "Borland Makefiles")
    # further settings required? not tested for a very long time!
    SET(CMAKE_STANDARD_LIBRARIES "import32.lib cw32mt.lib")
  ENDIF(CMAKE_GENERATOR MATCHES "Borland Makefiles")

#ENDIF(DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS AND NOT WITH_SHARED_LIBS)
ENDIF(ITK_DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS)
endmacro(mimic_dcmtk_compiler_flags)
# ----------------------------------------------------------




# Make decisions based on DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS


option(USE_SYSTEM_DCMTK "Should we use a version of DCMTK already installed on this machine?" ON)

# External build of DCMTK if desired
if(USE_SYSTEM_DCMTK)
  set(DCMTK_DIR CACHE PATH "Location of the /bin, /include and /lib directories for DCMTK")
  
  message("Checking if DCMTK_DIR (${DCMTK_DIR}) is a build tree")
  find_path(DCMTK_Local_Build CMakeCache.txt PATHS "${DCMTK_DIR}" NO_DEFAULT_PATH )
  if (DCMTK_Local_Build)
    message("Found ITK_DCMTK build directory ${DCMTK_Local_Build}")
    
    # Call our function to load the DCMTK cache values
    LoadDCMTKCache( ${DCMTK_Local_Build} )

    
    message("Looking for BIN DIR ${ITK_DCMTK_BINARY_DIR}")
    find_path(WJR_DCMTK_BINARY_DIR CMakeCache.txt PATHS ${ITK_DCMTK_BINARY_DIR} NO_DEFAULT_PATH)
    if (WJR_DCMTK_BINARY_DIR)
      message("Found it - ${ITK_DCMTK_BINARY_DIR}")
    else()
      message("${ITK_DCMTK_BINARY_DIR} was not found")
    endif()
  else(DCMTK_Local_Build)
    # Since the DCMTK_DIR does not contain a CMakeCache.txt file, we can assume
    # this directory in the installed path
    if(NOT DCMTK_DIR)
      # Look for the library files
      find_library(TMP_LIB dcmdata)
      message("TMP_LIB is ${TMP_LIB}")
      if(EXISTS "${TMP_LIB}")
        message("...inside TMP_LIB condition")
        get_filename_component(MY_PATH ${TMP_LIB} PATH CACHE)
        message("Looking for installed dir for DCMTK_DIR and found [${MY_PATH}]")
        
        ## Strip the "/lib" off the end (and try to make it case insensitive)
        string(TOUPPER "${MY_PATH}" upperstr)
        string(REGEX REPLACE "/LIB$" "" LIBBASE "${upperstr}")
        message("LIBBASE (after stripping /lib) is [${LIBBASE}]")
        string(LENGTH "${LIBBASE}" base_length)
        # Extract the proper number of chars from the original (case sensitive) path
        string(SUBSTRING "${MY_PATH}" 0 ${base_length} case_sensitive_base)
        message("Case Sensitive Base Path is [${case_sensitive_base}]")
        # Save the directory
        set(DCMTK_DIR ${case_sensitive_base} CACHE PATH "Location of the /bin, /include and /lib directories for DCMTK" FORCE)
        message("### LIB files will be searched for in ${DCMTK_DIR}/lib")
      endif()
    endif()
  endif(DCMTK_Local_Build)
  
### We may need the following for Linux builds from DCMTK source/installed
#  SET(USE_STD_CXX_INCLUDES 1)
#  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DUSE_STD_CXX_INCLUDES)
  

  if(WIN32)
    list(APPEND DCMTK_LIBRARIES netapi32 wsock32)
  endif()

  if(DCMTK_ofstd_INCLUDE_DIR)
    get_filename_component(DCMTK_dcmtk_INCLUDE_DIR
      ${DCMTK_ofstd_INCLUDE_DIR}
      PATH
      CACHE)
    list(APPEND DCMTK_INCLUDE_DIRS ${DCMTK_dcmtk_INCLUDE_DIR})
    mark_as_advanced(DCMTK_dcmtk_INCLUDE_DIR)
  endif()
    
    
    
    
    #find_package(DCMTK 3.6 REQUIRED PATHS ${DCMTK_DIR})
    #find_package(DCMTK 3.6 PATHS ${DCMTK_DIR})
  #  if(NOT DCMTK_FOUND)
  #    MESSAGE("DCMTK not found with find_package() command")
  #  else()
  #    MESSAGE("DCMTK was found.  DCMTK_LIBRARIES is ${DCMTK_LIBRARIES}")
  #  endif()
else()
  ###
  ###  Build with:  cmake .. -DUSE_SYSTEM_DCMTK:BOOL=OFF
  ###

  # include(External_DCMTK)
  ExternalProject_Add( ITK_DCMTK
    GIT_REPOSITORY http://git.dcmtk.org/dcmtk.git
    INSTALL_COMMAND ""
    # INSTALL ITK_DCMTK_INSTALL
  )
  
  # Try to make sure CMake runs on our main project AFTER DCMTK is built
  ExternalProject_Add_Step(ITK_DCMTK d1
    COMMAND ${CMAKE_COMMAND} ${CMAKE_BINARY_DIR}
    COMMENT "Refreshing build configuration after DCMTK has been created"
    DEPENDEES install
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
  
  # Try to make sure CMake runs on our main project AFTER DCMTK is built
#  ExternalProject_Add_Step(ITK_DCMTK d2
#    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ITK_DCMTK --config Release
#    COMMENT "Rebuilding in step d2"
#    DEPENDEES d1
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#  )
  
  add_dependencies(ext3 ITK_DCMTK)
  
  # Can we compile the library during CMake time?
#  try_compile(RESULT_VAR "${ep_base}/Build/ITK_DCMTK" "${ep_base}/Source/ITK_DCMTK"
#    ITK_DCMTK 
#    OUTPUT_VARIABLE DCMTK_DID_BUILD)
  
  
  message("Looking for ITK_DCMTK build directory ${ep_base}/Build/ITK_DCMTK")
  find_path(DCMTK_Local_Build CMakeCache.txt PATHS "${ep_base}/Build/ITK_DCMTK" NO_DEFAULT_PATH )
  if (DCMTK_Local_Build)
    message("Found ITK_DCMTK build directory ${DCMTK_Local_Build}")

    # Call our function to load the DCMTK cache values
    LoadDCMTKCache( ${DCMTK_Local_Build} )
    
     
    message("Looking for BIN DIR ${ITK_DCMTK_BINARY_DIR}")
    find_path(WJR_DCMTK_BINARY_DIR CMakeCache.txt PATHS ${ITK_DCMTK_BINARY_DIR} NO_DEFAULT_PATH)
    if (WJR_DCMTK_BINARY_DIR)
      message("Found it - ${ITK_DCMTK_BINARY_DIR}")
    else()
      message("${ITK_DCMTK_BINARY_DIR} was not found")
    endif()
    #
    # -- The following line fails with this error:
    # -- get_property could not find TARGET DCMTK.  Perhaps it has not yet been created.
    #
    #get_property(WJR_DIRS TARGET DCMTK PROPERTY INCLUDE_DIRECTORIES)
    #message("WJR_DIRS looking for INCLUDE_DIRECTORIES is:  ${WJR_DIRS}")
    
    
    # Save the directory
    set(DCMTK_DIR ${ITK_DCMTK_BINARY_DIR} CACHE PATH "Location of the /bin, /include and /lib directories for DCMTK")
    
    
  endif()
  
  
  
  # --------------------
  # Other possibilities:
  # --------------------
  # ftp://dicom.offis.de/pub/dicom/offis/software/dcmtk/dcmtk360/support/zlib-1.2.5.tar.gz
  # ftp://dicom.offis.de/pub/dicom/offis/software/dcmtk/dcmtk360/support/tiff-3.9.4.tar.gz
  # ftp://dicom.offis.de/pub/dicom/offis/software/dcmtk/dcmtk360/support/libpng-1.4.3.tar.gz
  # ftp://dicom.offis.de/pub/dicom/offis/software/dcmtk/dcmtk360/support/libxml2-2.7.7.tar.gz
  # ftp://dicom.offis.de/pub/dicom/offis/software/dcmtk/dcmtk360/support/libiconv-1.13.1.tar.gz
  
  
  
endif()

message("ep_base is ${ep_base}")

message("DCMTK_FOUND is ${DCMTK_FOUND}")

message("DCMTK_DIR is ${DCMTK_DIR}")
message("DCMTK_INCLUDE_DIRS is ${DCMTK_INCLUDE_DIRS}")


####################################################################
####################################################################

# We've chosen our source types above.  Any caches have now been loaded.
# Now we can check some values
message("ITK_DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS is set to ${ITK_DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS}")
mimic_dcmtk_compiler_flags()

message("Looking for DCMTK libraries for linking")

  foreach(lib
      charls
      dcmdata
      dcmdsig
      dcmimage
      dcmimgle
      dcmjpeg
      dcmjpls
      dcmnet
      dcmpstat
      dcmqrdb
      dcmsr
      dcmtls
      dcmwlm
      i2d
      ijg12
      ijg16
      ijg8
      oflog
      ofstd )

    find_library(DCMTK_${lib}_LIBRARY
      ${lib}
      PATHS
      ${DCMTK_DIR}/lib               # Typical for INSTALL directory
      ${DCMTK_DIR}/${lib}/libsrc     # External build tree (for most libs)
      ${DCMTK_DIR}/dcmsign/libsrc    # External build tree (for dcmdsig, dirname does not match libname)
      ${DCMTK_DIR}/dcmjpeg/lib${lib} # External build tree (ijg8, ijg12, ijg16)
      ${DCMTK_DIR}/dcmdata/lib${lib} # External build tree (i2d)
      ${DCMTK_DIR}/dcmjpls/lib${lib} # External build tree (charls)
      NO_DEFAULT_PATH
    )

    mark_as_advanced(DCMTK_${lib}_LIBRARY)

    if(DCMTK_${lib}_LIBRARY)
      list(APPEND DCMTK_LIBRARIES ${DCMTK_${lib}_LIBRARY})
    elseif(CMAKE_CONFIGURATION_TYPES)
      # Library was not found above, look under each build type
      foreach(configtype ${CMAKE_CONFIGURATION_TYPES})
        find_library(DCMTK_${lib}_LIBRARY_${configtype}
          ${lib}
          PATHS
          ${DCMTK_DIR}/${lib}/libsrc/${configtype}     # External build tree (for most libs)
          ${DCMTK_DIR}/dcmsign/libsrc/${configtype}    # External build tree (for dcmdsig, dirname does not match libname)
          ${DCMTK_DIR}/dcmjpeg/lib${lib}/${configtype} # External build tree (ijg8, ijg12, ijg16)
          ${DCMTK_DIR}/dcmdata/lib${lib}/${configtype} # External build tree (i2d)
          ${DCMTK_DIR}/dcmjpls/lib${lib}/${configtype} # External build tree (charls)
        )
        
        if (DCMTK_${lib}_LIBRARY_${configtype})
          # Check to see if we've already added the library
          list(FIND DCMTK_LIBRARIES DCMTK_${lib}_IMP list_index)
          if ( list_index EQUAL "-1" )
            # Create a new entry
            message("Creating library DCMTK_${lib}_LIBRARY")
            add_library(DCMTK_${lib}_IMP STATIC IMPORTED)
            list(APPEND DCMTK_LIBRARIES DCMTK_${lib}_IMP)
          endif()
          
          # Now set the config-specific location properties
          string(TOUPPER ${configtype} UPPER_CONFIG_TYPE)
          message("Setting DCMTK_${lib}_IMP PROPERTY IMPORTED_LOCATION_${UPPER_CONFIG_TYPE} = ${DCMTK_${lib}_LIBRARY_${configtype}}")
          set_property(TARGET DCMTK_${lib}_IMP PROPERTY IMPORTED_LOCATION_${UPPER_CONFIG_TYPE} ${DCMTK_${lib}_LIBRARY_${configtype}})
          
          
#          # Debug build libraries get a special case
#          if ( ${configtype} STREQUAL "Debug" )
#            list(APPEND DCMTK_LIBRARIES_${configtype} ${DCMTK_${lib}_LIBRARY_${configtype}})
#          else()
#            list(APPEND DCMTK_LIBRARIES ${DCMTK_${lib}_LIBRARY_${configtype}})
#          endif()
        endif()
      endforeach()
    endif()
  endforeach()
  
message("DCMTK_LIBRARIES is ${DCMTK_LIBRARIES}")

target_link_libraries(ext3 ${DCMTK_LIBRARIES})



######################################
# Search for DCMTK Include Directories
######################################
if(DCMTK_DIR)
  set(DCMTK_config_TEST_HEADER osconfig.h)
  set(DCMTK_dcmdata_TEST_HEADER dctypes.h)
  set(DCMTK_dcmimage_TEST_HEADER dicoimg.h)
  set(DCMTK_dcmimgle_TEST_HEADER dcmimage.h)
  set(DCMTK_dcmjpeg_TEST_HEADER djdecode.h)
  set(DCMTK_dcmnet_TEST_HEADER assoc.h)
  set(DCMTK_dcmpstat_TEST_HEADER dcmpstat.h)
  set(DCMTK_dcmqrdb_TEST_HEADER dcmqrdba.h)
  set(DCMTK_dcmsign_TEST_HEADER sicert.h)
  set(DCMTK_dcmsr_TEST_HEADER dsrtree.h)
  set(DCMTK_dcmtls_TEST_HEADER tlslayer.h)
  set(DCMTK_ofstd_TEST_HEADER ofstdinc.h)

  foreach(dir
      config
      dcmdata
      dcmimage
      dcmimgle
      dcmjpeg
      dcmnet
      dcmpstat
      dcmqrdb
      dcmsign
      dcmsr
      dcmtls
      ofstd)
      
    set(incfile dcmtk/${dir}/${DCMTK_${dir}_TEST_HEADER})
    message("Looking for file ${incfile} in the following locations:")
    message("    ${DCMTK_DIR}/include/${incfile}")
    message("    ${ITK_DCMTK_SOURCE_DIR}/${dir}/include/${incfile}")
    message("    ${ep_base}/Source/ITK_DCMTK/${dir}/include/${incfile}")
    
    find_path(DCMTK_${dir}_INCLUDE_DIR
      ${incfile}
      PATHS
      ${DCMTK_DIR}/include                        # Installed Path
      ${ITK_DCMTK_SOURCE_DIR}/${dir}/include      # Source for the local build
      ${ep_base}/Source/ITK_DCMTK/${dir}/include  # When built as ExternalProject_add()
      NO_DEFAULT_PATH
      )

    mark_as_advanced(DCMTK_${dir}_INCLUDE_DIR)
    message("DCMTK_${dir}_INCLUDE_DIR is ${DCMTK_${dir}_INCLUDE_DIR}")

    if(DCMTK_${dir}_INCLUDE_DIR)
      list(APPEND
        DCMTK_INCLUDE_DIRS
        ${DCMTK_${dir}_INCLUDE_DIR})
    endif()
  endforeach(dir)

  # Clean up the include list
  list(REMOVE_DUPLICATES DCMTK_INCLUDE_DIRS)
  
  message("INFO:  FINAL DCMTK_INCLUDE_DIRS: ${DCMTK_INCLUDE_DIRS}")
  
  # Add to our project
  include_directories(${DCMTK_INCLUDE_DIRS})
endif()

# Final changes
#  set(CMAKE_CXX_FLAGS '${CMAKE_CXX_FLAGS} -DUSE_STD_CXX_INCLUDES')


