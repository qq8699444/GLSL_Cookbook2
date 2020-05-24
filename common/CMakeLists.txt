set ( common_src esUtil.cpp
				 esUtil_win32.cpp
				 ShaderUtil.cpp
				 vbotorus.cpp
				 glslprogram.cpp)



add_library( Common STATIC ${common_src}  )
target_link_libraries( Common ${OPENGLES3_LIBRARY} ${EGL_LIBRARY} )

             


