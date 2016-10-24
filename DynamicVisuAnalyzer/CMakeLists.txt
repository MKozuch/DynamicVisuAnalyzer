cmake_minimum_required(VERSION 3.0.2)
cmake_policy(SET CMP0020 NEW)
project(DynamicVisuAnalyzerMainWin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(QT_USE_QTMAIN TRUE)

set(SOURCE_FILES 
	main.cpp
	dynamicvisuanalyzer.cpp
	dvadata.cpp
	)

set(HEADER_FILES
	dynamicvisuanalyzer.h
	dvadata.h
	dvaplugininterface.h
	)
	
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(GDCM REQUIRED)
include(${GDCM_USE_FILE})

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Core REQUIRED)

add_executable(DynamicVisuAnalyzer ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(DynamicVisuAnalyzer ${QT_LIBRARIES} ${VTK_LIBRARIES} ${ITK_LIBRARIES} ${GDCM_LIBRARIES}
	vtkgdcm)

INSTALL(TARGETS DynamicVisuAnalyzer RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})