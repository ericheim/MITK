# This file is intended to be included at the end of a custom MITKDashboardScript.TEMPLATE.cmake file

list(APPEND CTEST_NOTES_FILES "${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}")

#
# Automatically determined properties
#
set(MY_OPERATING_SYSTEM "${CMAKE_HOST_SYSTEM}") # Windows 7, Linux-2.6.32, Darwin... 
site_name(CTEST_SITE)

if(QT_BINARY_DIR)
  set(QT_QMAKE_EXECUTABLE "${QT_BINARY_DIR}/qmake")
else()
  set(QT_QMAKE_EXECUTABLE "qmake")
endif()

execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} --version
                OUTPUT_VARIABLE MY_QT_VERSION
                RESULT_VARIABLE qmake_error)
if(qmake_error)
  message(FATAL_ERROR "Error when executing ${QT_QMAKE_EXECUTABLE} --version\n${qmake_error}")
endif()

string(REGEX REPLACE ".*Qt version ([0-9.]+) .*" "\\1" MY_QT_VERSION ${MY_QT_VERSION})

#
# Project specific properties
#
if(NOT CTEST_BUILD_NAME)
  set(CTEST_BUILD_NAME "${MY_OPERATING_SYSTEM}-${MY_COMPILER}-Qt-${MY_QT_VERSION}-${CTEST_BUILD_CONFIGURATION}")
endif()
set(PROJECT_BUILD_DIR "MITK-build")

set(CTEST_PATH "$ENV{PATH}")
if(WIN32)
  set(VTK_BINARY_DIR "${CTEST_BINARY_DIRECTORY}/VTK-build/bin/${CTEST_BUILD_CONFIGURATION}")
  set(ITK_BINARY_DIR "${CTEST_BINARY_DIRECTORY}/ITK-build/bin/${CTEST_BUILD_CONFIGURATION}")
  set(GDCM_BINARY_DIR "${CTEST_BINARY_DIRECTORY}/GDCM-build/bin/${CTEST_BUILD_CONFIGURATION}")
  set(BLUEBERRY_OSGI_DIR "${CTEST_BINARY_DIRECTORY}/MITK-build/bin/BlueBerry/org.blueberry.osgi/bin/${CTEST_BUILD_CONFIGURATION}")
  set(CTEST_PATH "${CTEST_PATH};${QT_BINARY_DIR};${VTK_BINARY_DIR};${ITK_BINARY_DIR};${GDCM_BINARY_DIR};${BLUEBERRY_OSGI_DIR}")
endif()
set(ENV{PATH} "${CTEST_PATH}")

set(SUPERBUILD_TARGETS "")

# If the dashscript doesn't define a GIT_REPOSITORY variable, let's define it here.
if (NOT DEFINED GIT_REPOSITORY OR GIT_REPOSITORY STREQUAL "")
  set(GIT_REPOSITORY "http://git.mitk.org/MITK.git")
endif()

#
# Display build info
#
message("Site name: ${CTEST_SITE}")
message("Build name: ${CTEST_BUILD_NAME}")
message("Script Mode: ${SCRIPT_MODE}")
message("Coverage: ${WITH_COVERAGE}, MemCheck: ${WITH_MEMCHECK}")


#
# Download and include dashboard driver script 
#
if(NOT DEFINED GIT_BRANCH OR GIT_BRANCH STREQUAL "")
  set(hb "HEAD")
else()
  set(hb "refs/heads/${GIT_BRANCH}")
endif()
set(url "http://mbits/gitweb/?p=MITK.git;a=blob_plain;f=CMake/MITKDashboardDriverScript.cmake;hb=${hb}")
set(dest ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}.driver)
downloadFile("${url}" "${dest}")
include(${dest})


