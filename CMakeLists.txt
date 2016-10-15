cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(DynamicVisuAnalyzer)

set_property(GLOBAL PROPERTY USE_FOLDERS On) 

set(VTK_DIR %VTK_DIR%)
set(ITK_DIR %ITK_DIR%)
set(GDCM_DIR %GDCM_DIR%)

add_subdirectory(DynamicVisuAnalyzer)
add_subdirectory(dvaExampleAlgo1)
add_subdirectory(dvaFourPaneView)