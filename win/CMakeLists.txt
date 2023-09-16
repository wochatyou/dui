# the build for Windows X64
project(duiapp-win64 CXX)

set(DUIAPP_DIR "${CMAKE_CURRENT_LIST_DIR}"
    CACHE PATH "DUI application directory")

set(BLEND2D_DIR "${DUIAPP_DIR}/../blend2d"
    CACHE PATH "Location of 'blend2d'")

# Enable Blend2D static build.
set(BLEND2D_STATIC TRUE)

include("${BLEND2D_DIR}/CMakeLists.txt")

add_executable(${PROJECT_NAME} WIN32 main.cpp duiapp.cpp duiapp.rc)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR} ${CMAKE_SOURCE_DIR} ${BLEND2D_DIR} {LIBDUI_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} blend2d::blend2d libdui)

set_property(TARGET ${PROJECT_NAME} PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
