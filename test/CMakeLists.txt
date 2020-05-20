add_executable(getStart getStart.cpp esUtil.cpp esUtil_win32.cpp ShaderUtil.cpp)
target_link_libraries(getStart ${OPENGLES3_LIBRARY} ${EGL_LIBRARY} )

add_executable(usingUniformBlocks usingUniformBlocks.cpp esUtil.cpp esUtil_win32.cpp ShaderUtil.cpp)
target_link_libraries(usingUniformBlocks ${OPENGLES3_LIBRARY} ${EGL_LIBRARY} )



