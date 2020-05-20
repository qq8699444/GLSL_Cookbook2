cmake_minimum_required( VERSION 2.6 )
project( GLSL_cookbook_note )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

#set_property(GLOBAL PROPERTY USE_FOLDERS On)
include_directories( D:/PowerVR_Graphics/PowerVR_SDK/SDK_2019_R2/include)
include_directories( common )
include_directories(D:/vulkan_dev/glm/)
#find_library( OPENGLES3_LIBRARY GLESv2 "OpenGL ES v3.0 library")
find_library( OPENGLES3_LIBRARY libGLESv2.lib D:/PowerVR_Graphics/PowerVR_SDK/SDK_2019_R2/lib/Windows_x86_64/  )
find_library( EGL_LIBRARY libEGL.lib D:/PowerVR_Graphics/PowerVR_SDK/SDK_2019_R2/lib/Windows_x86_64/ )
#find_library( PVRScopeDeveloper PVRScopeDeveloper.lib  D:/PowerVR_Graphics/PowerVR_SDK/SDK_2019_R2/lib/Windows_x86_64/ )
SUBDIRS( common ch1 )		