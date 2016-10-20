cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(DynamicVisuAnalyzer)

set_property(GLOBAL PROPERTY USE_FOLDERS On) 

set(VTK_DIR %VTK_DIR%)
set(ITK_DIR %ITK_DIR%)
set(GDCM_DIR %GDCM_DIR%)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(DynamicVisuAnalyzer)
add_subdirectory(dvaExampleAlgo1)
add_subdirectory(dvaFourPaneView)
add_subdirectory(dvaDicomInfo)

install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/ DESTINATION /. )