cmake_minimum_required(VERSION 3.0.2)
cmake_policy(SET CMP0020 NEW)
project(dvaExampleAlgo1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(SOURCE_FILES 
	dvaExampleAlgo1.cpp
	../DynamicVisuAnalyzer/dvadata.cpp
	)

set(HEADER_FILES
	dvaExampleAlgo1.h
	dvaexamplealgo1_global.h
	../DynamicVisuAnalyzer/dvaplugininterface.h
	../DynamicVisuAnalyzer/dvadata.h
	)
	
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_SHARED)
	
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Core REQUIRED)

add_library(dvaExampleAlgo1 SHARED ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(dvaExampleAlgo1 ${QT_LIBRARIES} ${VTK_LIBRARIES} ${ITK_LIBRARIES})