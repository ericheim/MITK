#
# Create a core library (Algorithms, DataStructures etc.)
# 
# No parameters. Expects files.cmake in the same directory and CMake
# variables LIBPOSTFIX and KITNAME to be set correctly
#

MACRO(ORGANIZE_SOURCES)

  # this macro gets a filelist as input and looks
  # for corresponding h-files to add them to the project.
  
  # additionally files are grouped in source-groups.
  
  # No parameters explicitly declared here, because
  # we want to allow for variable argument lists, which
  # are later access by the keyword FOREACH(MYFILE ${ARGV})
  
  # output: after calling the macro, files that were found
  # correspondigly to the given files are stored in the
  # variable:
  # ${CORRESPONDING_H_FILES}
  # ${CORRESPONDING_TXX_FILES}
  # ${CORRESPONDING_UI_H_FILES}
  
  # Globbed can be found in the variables
  # ${GLOBBED_TXX_FILES} (CURRENTLY COMMENTED OUT)
  # ${GLOBBED_DOX_FILES}

  MACRO_PARSE_ARGUMENTS(_ORG "HEADER;SOURCE;TXX;DOC;MOC;GEN_QRC;GEN_UI;UI;QRC" "" ${ARGN})

  SET(CORRESPONDING__H_FILES "" )
  SET(GLOBBED__H_FILES "" )
  
  IF(_ORG_HEADER)
    FOREACH(_file ${_ORG_SOURCE})
      STRING(REGEX REPLACE "(.*)\\.(txx|cpp|c|cxx)$" "\\1.h" H_FILE ${_file})
      IF(EXISTS ${H_FILE})
        LIST(APPEND CORRESPONDING__H_FILES "${H_FILE}")
  	  ENDIF()
    ENDFOREACH()
  ELSE()
    FILE(GLOB_RECURSE GLOBBED__H_FILES *.h)
  ENDIF()
  
  IF(_ORG_GEN_QRC OR _ORG_GEN_UI OR _ORG_MOC)
    SOURCE_GROUP("Generated\\Qt QRC Source Files" FILES ${_ORG_GEN_QRC})
    SOURCE_GROUP("Generated\\Qt UI Header Files" FILES ${_ORG_GEN_UI})
    SOURCE_GROUP("Generated\\Qt MOC Source Files" FILES ${_ORG_MOC})
  ENDIF()
  
  #_MACRO_APPEND_TO_LIST(_ORG_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/")
  SOURCE_GROUP("== Source Files ==" FILES ${_ORG_SOURCE})
  
  #_MACRO_APPEND_TO_LIST(_ORG_TXX "${CMAKE_CURRENT_SOURCE_DIR}/")
  SOURCE_GROUP("== Template Files ==" FILES ${_ORG_TXX})
  
  #_MACRO_APPEND_TO_LIST(_ORG_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/")
  SOURCE_GROUP("== Header Files ==" FILES ${_ORG_HEADER} ${_ORG_HEADER} ${CORRESPONDING__H_FILES} ${GLOBBED__H_FILES})
  
  
  #_MACRO_APPEND_TO_LIST(_ORG_UI "${CMAKE_CURRENT_SOURCE_DIR}/")
  SOURCE_GROUP("QT UI Files" FILES ${_ORG_UI})
  
  #_MACRO_APPEND_TO_LIST(_ORG_DOC "${CMAKE_CURRENT_SOURCE_DIR}/")
  SOURCE_GROUP("Doxygen Files" FILES ${_ORG_DOC})
  
  #_MACRO_APPEND_TO_LIST(_ORG_QRC "${CMAKE_CURRENT_SOURCE_DIR}/")
  SOURCE_GROUP("Qt Resource Files" FILES ${_ORG_QRC})
  
ENDMACRO(ORGANIZE_SOURCES)

MACRO(APPLY_VTK_FLAGS)
  IF(NOT MITK_VTK_FLAGS_APPLIED)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${VTK_REQUIRED_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VTK_REQUIRED_CXX_FLAGS}")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${VTK_REQUIRED_EXE_LINKER_FLAGS}")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${VTK_REQUIRED_SHARED_LINKER_FLAGS}")
    SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${VTK_REQUIRED_MODULE_LINKER_FLAGS}")
    SET(MITK_VTK_FLAGS_APPLIED 1)
  ENDIF(NOT MITK_VTK_FLAGS_APPLIED)
ENDMACRO(APPLY_VTK_FLAGS)

# increase heap limit for MSVC70. Assumes /Zm1000 is set by ITK
MACRO(INCREASE_MSVC_HEAP_LIMIT)
IF(MSVC70)
 STRING(REPLACE /Zm1000 /Zm1200 CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})  
ENDIF(MSVC70)
ENDMACRO(INCREASE_MSVC_HEAP_LIMIT)

# suppress some warnings in VC8 about using unsafe/deprecated c functions
MACRO(SUPPRESS_VC8_DEPRECATED_WARNINGS)
IF(MSVC80)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS)
ENDIF(MSVC80)
ENDMACRO(SUPPRESS_VC8_DEPRECATED_WARNINGS)

INCLUDE(${CMAKE_ROOT}/Modules/TestCXXAcceptsFlag.cmake)
MACRO(CHECK_AND_SET flag sourcelist )
  CHECK_CXX_ACCEPTS_FLAG(${flag} R)
  IF(R)
    SET_SOURCE_FILES_PROPERTIES(
      ${${sourcelist}}
       PROPERTIES
       COMPILE_FLAGS ${flag}
      )
  ENDIF(R)
ENDMACRO(CHECK_AND_SET)

#
# MITK_MULTIPLEX_PICTYPE: generate separated source files for different
# data types to reduce memory consumption of compiler during template 
# instantiation
#
# Param "file" should be named like mitkMyAlgo-TYPE.cpp
# in the file, every occurence of @TYPE@ is replaced by the
# datatype. For each datatype, a new file mitkMyAlgo-datatype.cpp 
# is generated and added to CPP_FILES_GENERATED. 
#
MACRO(MITK_MULTIPLEX_PICTYPE file)
  SET(TYPES "double;float;int;unsigned int;short;unsigned short;char;unsigned char")
  FOREACH(TYPE ${TYPES})
    # create filename for destination
    STRING(REPLACE " " "_" quoted_type "${TYPE}")
    STRING(REPLACE TYPE ${quoted_type} quoted_file ${file})
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/${file} ${CMAKE_CURRENT_BINARY_DIR}/${quoted_file} @ONLY)
    SET(CPP_FILES_GENERATED ${CPP_FILES_GENERATED} ${CMAKE_CURRENT_BINARY_DIR}/${quoted_file})
  ENDFOREACH(TYPE)
ENDMACRO(MITK_MULTIPLEX_PICTYPE)

INCLUDE(MacroParseArguments)

MACRO(MITK_CREATE_MODULE_CONF MODULE_NAME_IN)
  MACRO_PARSE_ARGUMENTS(MODULE "INCLUDE_DIRS;DEPENDS;PROVIDES" "QT_MODULE" ${ARGN})
  SET(MODULE_NAME ${MODULE_NAME_IN})
  _MITK_CREATE_MODULE_CONF()
ENDMACRO(MITK_CREATE_MODULE_CONF)

MACRO(_MITK_CREATE_MODULE_CONF)
  IF(NOT MODULE_PROVIDES)
    SET(MODULE_PROVIDES ${MODULE_NAME})
  ENDIF(NOT MODULE_PROVIDES)
  SET(MODULE_INCLUDE_DIRS_ABSOLUTE "")
  FOREACH(dir ${MODULE_INCLUDE_DIRS})
    GET_FILENAME_COMPONENT(abs_dir ${dir} ABSOLUTE)
    SET(MODULE_INCLUDE_DIRS_ABSOLUTE ${MODULE_INCLUDE_DIRS_ABSOLUTE} ${abs_dir})
  ENDFOREACH(dir)

  SET(MODULE_INCLUDE_DIRS ${MODULE_INCLUDE_DIRS_ABSOLUTE} ${CMAKE_CURRENT_SOURCE_DIR})
  
  # Qt generates headers in the binary tree
  IF(MODULE_QT_MODULE)
    SET(MODULE_INCLUDE_DIRS ${MODULE_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
  ENDIF(MODULE_QT_MODULE)
  
  CONFIGURE_FILE(${MITK_SOURCE_DIR}/CMake/moduleConf.cmake.in ${MITK_MODULES_CONF_DIR}/${MODULE_NAME}Config.cmake @ONLY)
ENDMACRO(_MITK_CREATE_MODULE_CONF)

##################################################################
#
# MITK_CREATE_MODULE 
#
# Creates a module for the automatic module dependeny system within MITK.
# Configurations are generated in the moduleConf directory. 
#
# USAGE:
#
# MITK_CREATE_MODULE( <moduleName>
#     [INCLUDE_DIRS <include directories>]
#     [INTERNAL_INCLUDE_DIRS <internally used include directories>]
#     [DEPENDS <modules we need>]
#     [PROVIDES <library name which is built>]
#     [PACKAGE_DEPENDS <packages we need, like ITK, VTK, QT>]
#     [EXPORT_DEFINE <declspec macro name for dll exports>]

MACRO(MITK_CREATE_MODULE MODULE_NAME_IN)
  MACRO_PARSE_ARGUMENTS(MODULE "INCLUDE_DIRS;INTERNAL_INCLUDE_DIRS;DEPENDS;DEPENDS_INTERNAL;PROVIDES;PACKAGE_DEPENDS;EXPORT_DEFINE;ADDITIONAL_LIBS;GENERATED_CPP" "QT_MODULE;FORCE_STATIC" ${ARGN})
  SET(MODULE_NAME ${MODULE_NAME_IN})
  
  # first of all we check for the dependencies
  MITK_CHECK_MODULE(_MISSING_DEP ${MODULE_DEPENDS})
  IF(_MISSING_DEP)
    MESSAGE("Warning: Module ${MODULE_NAME} won't be built, missing dependency: ${_MISSING_DEP}") 
     # remove possibly existing config, to avoid strange errors
     FILE(REMOVE ${MITK_MODULES_CONF_DIR}/${MODULE_NAME}Config.cmake )
   ELSE(_MISSING_DEP)
  _MITK_CREATE_MODULE_CONF()
  
  IF(NOT MODULE_EXPORT_DEFINE)
    SET(MODULE_EXPORT_DEFINE ${MODULE_NAME}_EXPORT)
  ENDIF(NOT MODULE_EXPORT_DEFINE)
  CONFIGURE_FILE(${MITK_SOURCE_DIR}/CMake/moduleExports.h.in ${MITK_MODULES_CONF_DIR}/${MODULE_NAME}Exports.h @ONLY)
  
  IF(MITK_GENERATE_MODULE_DOT)
  MESSAGE("MODULEDOTNAME ${MODULE_NAME}")
  FOREACH(dep ${MODULE_DEPENDS})
    MESSAGE("MODULEDOT \"${MODULE_NAME}\" -> \"${dep}\" ; ")
  ENDFOREACH(dep)
  ENDIF(MITK_GENERATE_MODULE_DOT)
 
  SET(DEPENDS "${MODULE_DEPENDS}") 
  SET(DEPENDS_BEFORE "not initialized")
  SET(PACKAGE_DEPENDS "${MODULE_PACKAGE_DEPENDS}")
  MITK_USE_MODULE("${MODULE_DEPENDS}") 
  
  # ok, now create the module itself 
  INCLUDE_DIRECTORIES(. ${ALL_INCLUDE_DIRECTORIES})
  INCLUDE(files.cmake)
  
  IF(NOT MODULE_QT_MODULE)
    ORGANIZE_SOURCES(SOURCE ${CPP_FILES}
                     HEADER	${H_FILES}
					 TXX ${TXX_FILES}
					 DOC ${DOX_FILES}
					 )
    IF(MODULE_FORCE_STATIC)
      SET(_STATIC ${MITK_WIN32_FORCE_STATIC})
    ENDIF(MODULE_FORCE_STATIC)

    ADD_LIBRARY(${MODULE_PROVIDES} ${_STATIC} ${CPP_FILES} ${H_FILES} ${CPP_FILES_GENERATED} ${GLOBBED__H_FILES} ${CORRESPONDING__H_FILES} ${TXX_FILES} ${DOX_FILES} ${UI_FILES} ${TOOL_CPPS})
    IF(ALL_LIBRARIES)  
      TARGET_LINK_LIBRARIES(${MODULE_PROVIDES} ${ALL_LIBRARIES})
   ENDIF(ALL_LIBRARIES)  
  ELSE(NOT MODULE_QT_MODULE)

    INCLUDE(files.cmake)
  
  IF(UI_FILES)
      QT4_WRAP_UI(Q${KITNAME}_GENERATED_UI_CPP ${UI_FILES})
  ENDIF(UI_FILES)
  
  IF(MOC_H_FILES)
      QT4_WRAP_CPP(Q${KITNAME}_GENERATED_MOC_CPP ${MOC_H_FILES})
  ENDIF(MOC_H_FILES)
  
  SET(Q${KITNAME}_GENERATED_CPP ${Q${KITNAME}_GENERATED_CPP} ${Q${KITNAME}_GENERATED_UI_CPP} ${Q${KITNAME}_GENERATED_MOC_CPP})
  
  ORGANIZE_SOURCES(SOURCE ${CPP_FILES} 
                   HEADER ${H_FILES}
				   TXX ${TXX_FILES}
				   DOC ${DOX_FILES}
                   UI ${UI_FILES}
                   QRC ${RES_FILES}
				   MOC ${Q${KITNAME}_GENERATED_MOC_CPP}
				   GEN_UI ${Q${KITNAME}_GENERATED_UI_CPP})
   
  # MITK_GENERATE_TOOLS_LIBRARY(Qmitk${LIBPOSTFIX} "NO")
  
  ADD_LIBRARY(${MODULE_PROVIDES} ${CPP_FILES} ${CPP_FILES_GENERATED} ${Q${KITNAME}_GENERATED_CPP} ${CORRESPONDING__H_FILES} ${GLOBBED__H_FILES} ${TXX_FILES} ${DOX_FILES} ${UI_FILES} ${RES_FILES} ${TOOL_GUI_CPPS})
  TARGET_LINK_LIBRARIES(${MODULE_PROVIDES} ${QT_LIBRARIES} ${ALL_LIBRARIES} QVTK)

  ENDIF(NOT MODULE_QT_MODULE) 


  # install only if shared lib (for now)
  IF(NOT MODULE_FORCE_STATIC)
    MITK_INSTALL_TARGETS(${MODULE_PROVIDES})
  ENDIF(NOT MODULE_FORCE_STATIC)
ENDIF(_MISSING_DEP)
ENDMACRO(MITK_CREATE_MODULE)

MACRO(MITK_USE_MODULE)
  SET(DEPENDS "${ARGN}") 
  SET(DEPENDS_BEFORE "not initialized")
  WHILE(NOT "${DEPENDS}" STREQUAL "${DEPENDS_BEFORE}")
    SET(DEPENDS_BEFORE ${DEPENDS})
    FOREACH(dependency ${DEPENDS})
      INCLUDE(${MITK_MODULES_CONF_DIR}/${dependency}Config.cmake)
      LIST(APPEND DEPENDS ${${dependency}_DEPENDS})
      LIST(APPEND PACKAGE_DEPENDS ${${dependency}_PACKAGE_DEPENDS})
    ENDFOREACH(dependency) 
    IF(DEPENDS)
      LIST(REMOVE_DUPLICATES DEPENDS)
      LIST(SORT DEPENDS)
    ENDIF(DEPENDS)
    IF(PACKAGE_DEPENDS)
      LIST(REMOVE_DUPLICATES PACKAGE_DEPENDS)
      LIST(SORT PACKAGE_DEPENDS)
    ENDIF(PACKAGE_DEPENDS)
  ENDWHILE()
  # CMake Debug
  FOREACH(dependency ${DEPENDS} ${MODULE_DEPENDS_INTERNAL})
    INCLUDE(${MITK_MODULES_CONF_DIR}/${dependency}Config.cmake)
    SET(ALL_INCLUDE_DIRECTORIES ${ALL_INCLUDE_DIRECTORIES} ${${dependency}_INCLUDE_DIRS})
    SET(ALL_LIBRARIES ${ALL_LIBRARIES} ${${dependency}_PROVIDES})
  ENDFOREACH(dependency)

  FOREACH(package ${PACKAGE_DEPENDS})  
    INCLUDE(MITK_${package}_Config)
    #SET(ALL_INCLUDE_DIRECTORIES ${ALL_INCLUDE_DIRECTORIES} ${${package}_INCLUDE_DIRS})
    # SET(ALL_LIBRARIES ${ALL_LIBRARIES} ${${package}_LIBRARIES})
  ENDFOREACH(package)
  SET(ALL_LIBRARIES ${ALL_LIBRARIES} ${MODULE_ADDITIONAL_LIBS})
  SET(ALL_INCLUDE_DIRECTORIES ${ALL_INCLUDE_DIRECTORIES} ${MODULE_INCLUDE_DIRS} ${MODULE_INTERNAL_INCLUDE_DIRS} ${MITK_MODULES_CONF_DIR})

ENDMACRO(MITK_USE_MODULE)

# Usage: MITK_CHECK_MODULE(RESULT_VAR [dependencies ...] )
# check if all required modules exist and stores missing module names in RESULT_VAR.
MACRO(MITK_CHECK_MODULE RESULT_VAR)
  SET(${RESULT_VAR} "")
  SET(DEPENDS "${ARGN}") 
  SET(DEPENDS_BEFORE "not initialized")
  SET(PACKAGE_DEPENDS "")
  WHILE(NOT "${DEPENDS}" STREQUAL "${DEPENDS_BEFORE}")
    SET(DEPENDS_BEFORE ${DEPENDS})
    FOREACH(dependency ${DEPENDS})
	  IF(EXISTS "${MITK_MODULES_CONF_DIR}/${dependency}Config.cmake")
      INCLUDE(${MITK_MODULES_CONF_DIR}/${dependency}Config.cmake)
      LIST(APPEND DEPENDS ${${dependency}_DEPENDS})
      LIST(APPEND PACKAGE_DEPENDS ${${dependency}_PACKAGE_DEPENDS})
	  ELSE(EXISTS "${MITK_MODULES_CONF_DIR}/${dependency}Config.cmake")
        LIST(APPEND ${RESULT_VAR} ${dependency})
		LIST(REMOVE_DUPLICATES ${RESULT_VAR})
	  ENDIF(EXISTS "${MITK_MODULES_CONF_DIR}/${dependency}Config.cmake")
    ENDFOREACH(dependency) 
    LIST(REMOVE_DUPLICATES DEPENDS)
    LIST(REMOVE_DUPLICATES PACKAGE_DEPENDS)
    LIST(SORT DEPENDS)
    LIST(SORT PACKAGE_DEPENDS)
  ENDWHILE()
ENDMACRO(MITK_CHECK_MODULE)

#
# MITK specific cross plattform install macro
#
# Usage: MITK_INSTALL_TARGETS(target1 [target2] ....)
#
MACRO(MITK_INSTALL_TARGETS)
  IF(WIN32)
   INSTALL(TARGETS ${ARGN} 
     RUNTIME DESTINATION bin
   )
  ELSE()
   INSTALL(TARGETS ${ARGN} 
     RUNTIME DESTINATION bin
     LIBRARY DESTINATION lib
     ARCHIVE DESTINATION lib/static
   )
  ENDIF()
ENDMACRO(MITK_INSTALL_TARGETS)

MACRO(MITK_CREATE_MODULE_TESTS)
IF(BUILD_TESTING)
  INCLUDE(files.cmake)
  INCLUDE_DIRECTORIES(.)
  
  SET(CMAKE_TESTDRIVER_BEFORE_TESTMAIN "mitk::LogBackend::Register();")
  SET(CMAKE_TESTDRIVER_AFTER_TESTMAIN "mitk::LogBackend::Unregister();")

  CREATE_TEST_SOURCELIST(MODULETEST_SOURCE ${MODULE_NAME}TestDriver.cpp 
    ${MODULE_TESTS} ${MODULE_IMAGE_TESTS} ${MODULE_CUSTOM_TESTS}
    EXTRA_INCLUDE mitkLog.h
  )
  SET(TESTDRIVER ${MODULE_NAME}TestDriver)
  ADD_EXECUTABLE(${TESTDRIVER} ${MODULETEST_SOURCE})
  TARGET_LINK_LIBRARIES(${TESTDRIVER} ${MODULE_PROVIDES} ${ALL_LIBRARIES})
  #
  # Now tell CMake which tests should be run. This is done automatically 
  # for all tests in ${KITNAME}_TESTS and ${KITNAME}_IMAGE_TESTS. The IMAGE_TESTS
  # are run for each image in the TESTIMAGES list.
  #
  FOREACH( test ${MODULE_TESTS} )
    GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
    ADD_TEST(${TName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TESTDRIVER} ${TName})
  ENDFOREACH( test )

  FOREACH(image ${MODULE_TESTIMAGES} ${ADDITIONAL_TEST_IMAGES} )
    IF(EXISTS ${image})
      SET(IMAGE_FULL_PATH ${image})
    ELSE(EXISTS ${image})
      # todo: maybe search other paths as well
      # yes, please in mitk/Testing/Data, too
      SET(IMAGE_FULL_PATH ${MITK_SOURCE_DIR}/Core/Code/Testing/Data/${image})
    ENDIF(EXISTS ${image})

    IF(EXISTS ${IMAGE_FULL_PATH})
    FOREACH( test ${MODULE_IMAGE_TESTS} )
      GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
      GET_FILENAME_COMPONENT(ImageName ${IMAGE_FULL_PATH} NAME)
      ADD_TEST(${TName}_${ImageName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TESTDRIVER} ${TName} ${IMAGE_FULL_PATH})
    ENDFOREACH( test )
    ELSE(EXISTS ${IMAGE_FULL_PATH})
      MESSAGE("!!!!! No such file: ${IMAGE_FULL_PATH} !!!!!")
    ENDIF(EXISTS ${IMAGE_FULL_PATH})
  ENDFOREACH( image )

ENDIF(BUILD_TESTING)


ENDMACRO(MITK_CREATE_MODULE_TESTS)

