project(libdui)

file(GLOB LIBDUI_SRC "*.c" "*.cpp")

add_library(${PROJECT_NAME} ${LIBDUI_SRC})
set_property(TARGET ${PROJECT_NAME} PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_include_directories(${PROJECT_NAME} 
    PUBLIC ${PROJECT_SOURCE_DIR} 
           ${CMAKE_SOURCE_DIR}/deps
           ${CMAKE_SOURCE_DIR}/deps/freetype
           )

