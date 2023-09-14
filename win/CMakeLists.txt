# the build for Windows X64
project(dui-win64 CXX)

set(WOCHAT_DIR "${CMAKE_CURRENT_LIST_DIR}"
    CACHE PATH "DUI application directory")

add_executable(${PROJECT_NAME} WIN32 main.cpp dui.cpp dui.rc)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR} ${CMAKE_SOURCE_DIR} {LIBDUI_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} libdui)

set_property(TARGET ${PROJECT_NAME} PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
