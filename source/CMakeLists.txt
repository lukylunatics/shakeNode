project(shakeNode)

set(MAYA_VERSION 2023 CACHE STRING "Maya version")

set(SOURCE_FILES 
	"shakeNode.h"
	"shakeNodeRot.h"
	"shakeCommand.h"
	"perlinNoise.h"
	"shakeNode.cpp"
	"shakeNodeRot.cpp"
	"shakeCommand.cpp"
	"perlinNoise.cpp"
	"pluginMain.cpp"
)

# OS Specific environment setup
set(CUSTOM_DEFINITIONS "REQUIRE_IOSTREAM;_BOOL")
set(MAYA_INSTALL_BASE_SUFFIX "")
set(MAYA_INC_SUFFIX "include")
set(MAYA_LIB_SUFFIX "lib")
if(WIN32)
	# Windows
	set(MAYA_INSTALL_BASE_DEFAULT "C:/Program Files/Autodesk")
	set(CUSTOM_DEFINITIONS "${CUSTOM_DEFINITIONS};NT_PLUGIN")
elseif(APPLE)
	# Apple
	set(MAYA_INSTALL_BASE_DEFAULT "/Applications/Autodesk")
	set(MAYA_INC_SUFFIX "include")
	set(MAYA_LIB_SUFFIX "Maya.app/Contents/MacOS")
	set(CUSTOM_DEFINITIONS "${CUSTOM_DEFINITIONS};OSMac_")
else(WIN32)
	# Linux
	set(MAYA_INSTALL_BASE_DEFAULT "/usr/autodesk")
endif(WIN32)

set(MAYA_INSTALL_BASE_PATH ${MAYA_INSTALL_BASE_DEFAULT} CACHE STRING "Path with maya's installation, C:/Program Files/Autodesk")
set(LIBRARIES ${LIBRARIES} "Foundation" "OpenMaya" "OpenMayaAnim" "OpenMayaUI" "OpenMayaRender")
set(_MAYA_LOCATION ${MAYA_INSTALL_BASE_PATH}/maya${MAYA_VERSION}${MAYA_INSTALL_BASE_SUFFIX})
set(_PROJECT ${PROJECT_NAME})

include_directories(${_MAYA_LOCATION}/${MAYA_INC_SUFFIX})
link_directories(${_MAYA_LOCATION}/${MAYA_LIB_SUFFIX})
add_library(${_PROJECT} SHARED ${SOURCE_FILES})
target_link_libraries(${_PROJECT} ${LIBRARIES})

set_target_properties(${_PROJECT} PROPERTIES COMPILE_DEFINITIONS "${CUSTOM_DEFINITIONS}")
set_target_properties(${_PROJECT} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(${_PROJECT} PROPERTIES CLEAN_DIRECT_OUTPUT 1)

if(WIN32)
	set_target_properties(${_PROJECT} PROPERTIES SUFFIX ".mll" )
	set_target_properties(${_PROJECT} PROPERTIES LINK_FLAGS "/export:initializePlugin /export:uninitializePlugin" )
elseif(APPLE)
	set_target_properties(${_PROJECT} PROPERTIES PREFIX "" )
	set_target_properties(${_PROJECT} PROPERTIES SUFFIX ".bundle" )
else(WIN32)
	set_target_properties(${_PROJECT} PROPERTIES PREFIX "" )
endif(WIN32)
