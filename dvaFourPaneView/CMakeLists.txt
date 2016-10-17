cmake_minimum_required(VERSION 3.0.2)
cmake_policy(SET CMP0020 NEW)
project(dvaFourPaneView)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(SOURCE_FILES 
	dvafourpaneview.cpp
	../DynamicVisuAnalyzer/dvadata.cpp
	)

set(HEADER_FILES
	dvafourpaneview.h
	dvafourpaneview_global.h
	../DynamicVisuAnalyzer/dvaplugininterface.h
	../DynamicVisuAnalyzer/dvadata.h
	
	)
	
set(VTK_DIR %VTK_DIR%)
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Core REQUIRED)

add_library(dvaFourPaneView SHARED ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(dvaFourPaneView ${QT_LIBRARIES} ${VTK_LIBRARIES})

INSTALL(TARGETS dvaFourPaneView RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins)