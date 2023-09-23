# the build for Windows X64
project(duiapp-win64 CXX)

set(DUIAPP_DIR "${CMAKE_CURRENT_LIST_DIR}"
    CACHE PATH "DUI application directory")

add_executable(${PROJECT_NAME} WIN32 main.cpp duiapp.cpp duiapp.rc)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR} ${CMAKE_SOURCE_DIR} {LIBDUI_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} libdui)

set_property(TARGET ${PROJECT_NAME} PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_link_libraries(${PROJECT_NAME} debug c:/vcpkg/packages/harfbuzz_x64-windows-static/debug/lib/harfbuzz.lib)
target_link_libraries(${PROJECT_NAME} optimized c:/vcpkg/packages/harfbuzz_x64-windows-static/lib/harfbuzz.lib)

target_link_libraries(${PROJECT_NAME} debug c:/vcpkg/packages/freetype_x64-windows-static/debug/lib/freetyped.lib)
target_link_libraries(${PROJECT_NAME} optimized c:/vcpkg/packages/freetype_x64-windows-static/lib/freetype.lib)

target_link_libraries(${PROJECT_NAME} debug c:/vcpkg/packages/cairo_x64-windows-static/debug/lib/cairo.lib)
target_link_libraries(${PROJECT_NAME} optimized c:/vcpkg/packages/cairo_x64-windows-static/lib/cairo.lib)

target_link_libraries(${PROJECT_NAME} debug c:/vcpkg/packages/fontconfig_x64-windows-static/debug/lib/fontconfig.lib)
target_link_libraries(${PROJECT_NAME} optimized c:/vcpkg/packages/fontconfig_x64-windows-static/lib/fontconfig.lib)

target_link_libraries(${PROJECT_NAME} debug c:/vcpkg/packages/pixman_x64-windows-static/debug/lib/pixman-1.lib)
target_link_libraries(${PROJECT_NAME} optimized c:/vcpkg/packages/pixman_x64-windows-static/lib/pixman-1.lib)

target_link_libraries(${PROJECT_NAME} debug c:/vcpkg/packages/expat_x64-windows-static/debug/lib/libexpatdMT.lib)
target_link_libraries(${PROJECT_NAME} optimized c:/vcpkg/packages/expat_x64-windows-static/lib/libexpatMT.lib)

target_link_libraries(${PROJECT_NAME} debug c:/vcpkg/packages/libpng_x64-windows-static/debug/lib/libpng16d.lib)
target_link_libraries(${PROJECT_NAME} optimized c:/vcpkg/packages/libpng_x64-windows-static/lib/libpng16.lib)

target_link_libraries(${PROJECT_NAME} debug c:/vcpkg/packages/zlib_x64-windows-static/debug/lib/zlibd.lib)
target_link_libraries(${PROJECT_NAME} optimized c:/vcpkg/packages/zlib_x64-windows-static/lib/zlib.lib)

target_link_libraries(${PROJECT_NAME} debug c:/vcpkg/packages/bzip2_x64-windows-static/debug/lib/bz2d.lib)
target_link_libraries(${PROJECT_NAME} optimized c:/vcpkg/packages/bzip2_x64-windows-static/lib/bz2.lib)

target_link_libraries(${PROJECT_NAME} debug c:/vcpkg/packages/brotli_x64-windows-static/debug/lib/brotlidec-static.lib)
target_link_libraries(${PROJECT_NAME} debug c:/vcpkg/packages/brotli_x64-windows-static/debug/lib/brotlicommon-static.lib)
target_link_libraries(${PROJECT_NAME} optimized c:/vcpkg/packages/brotli_x64-windows-static/lib/brotlidec-static.lib)
target_link_libraries(${PROJECT_NAME} optimized c:/vcpkg/packages/brotli_x64-windows-static/lib/brotlicommon-static.lib)
